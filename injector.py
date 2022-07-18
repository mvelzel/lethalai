import subprocess
import pefile
from ctypes import *
from ctypes.wintypes import *

class MODULEENTRY32(Structure):
    _fields_ = [( 'dwSize' , DWORD ) , 
                ( 'th32ModuleID' , DWORD ),
                ( 'th32ProcessID' , DWORD ),
                ( 'GlblcntUsage' , DWORD ),
                ( 'ProccntUsage' , DWORD ) ,
                ( 'modBaseAddr' , POINTER(BYTE) ) ,
                ( 'modBaseSize' , DWORD ) , 
                ( 'hModule' , HMODULE ) ,
                ( 'szModule' , c_char * 256 ),
                ( 'szExePath' , c_char * 260 ) ]

class Injector:
    PROC_ALL_ACCESS = (0x000F0000 | 0x00100000 | 0x00000FFF)
    MEM_CREATE = 0x00001000 | 0x00002000
    MEM_RELEASE = 0x8000
    PAGE_EXECUTE_READWRITE = 0x40

    def __init__(self):
        self.kernel32 = windll.kernel32
        self.user32 = windll.user32
        self.pid = c_ulong()
        self.handle = None

    def create_process(self, path):
        return subprocess.Popen([path]).pid

    def load_from_pid(self, pid):
        self.unload()
        self.pid = c_ulong(pid)
        self.handle = self.kernel32.OpenProcess(self.PROC_ALL_ACCESS, 0, pid)
        if not self.handle:
            raise WinError()

    def unload(self):
        if self.handle:
            self.kernel32.CloseHandle(self.handle)
            if not self.handle:
                raise WinError()
        self.handle = None

    def alloc_remote(self, buffer, size):
        alloc = self.kernel32.VirtualAllocEx(self.handle, None, c_int(size),
                                             self.MEM_CREATE, self.PAGE_EXECUTE_READWRITE)
        if not alloc:
            raise WinError()
        self.write_memory(alloc, buffer)
        return alloc

    def free_remote(self, addr, size):
        if not self.kernel32.VirtualFreeEx(self.handle, addr, c_int(0), self.MEM_RELEASE):
            raise WinError()

    def get_address_from_module(self, module, function):
        function = function.encode('ascii')
        module_base = self.get_process_module_base(module.encode('ascii'))
        module_base = int.from_bytes(module_base, "little")
        pe = pefile.PE(module, fast_load=True)
        pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_EXPORT']])
#         for symbol in pe.DIRECTORY_ENTRY_EXPORT.symbols:
#             print(symbol.name)
        return [exp.address + module_base for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols if str(exp.name) == str(function)][0]
    
    def get_process_module_base(self,  full_module_path):
        """Get a the base address of a module belonging to a 32bit process, doing so from a 64bit process
        ***Does not work with processes created suspended, but does work with fully loaded suspended processess***
         
        for full_module_path we need full path and name to distinguish the right modules as there may be 2 or more modules 
        with the same name ie kernel32.dll, ntdll.dll 32 and 64bit versions etc"""
        TH32CS_SNAPMODULE32 = 0x00000010
        TH32CS_SNAPMODULE = 0x00000008
        hSnapshot = c_void_p(0)
        hSnapshot = self.kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, self.pid)
        module = MODULEENTRY32()
        module.dwSize = sizeof(MODULEENTRY32)
        ret = self.kernel32.Module32First(hSnapshot, pointer(module))
        if not ret:
            raise WinError()
        while ret:
            if str(module.szExePath).lower() == str(full_module_path).lower():
                return module.modBaseAddr
            ret = self.kernel32.Module32Next(hSnapshot, pointer(module))

        return None

    def create_remote_thread(self, function_addr, args):
        dll_addr = c_long(0)
        args_addr = self.alloc_remote(args, len(args))
        thread = self.kernel32.CreateRemoteThread(self.handle, None, None, c_long(function_addr),
                                                  c_long(args_addr), None, None)
        if not thread:
            raise WinError()
        if self.kernel32.WaitForSingleObject(thread, 0xFFFFFFFF) == 0xFFFFFFFF:
            raise WinError()
        if not self.kernel32.GetExitCodeThread(thread, byref(dll_addr)):
            raise WinError()
        self.free_remote(args_addr, len(args))
        return dll_addr.value
    
    def read_remote_thread(self, function_addr, args):
        dll_addr = c_long(0)
        args_addr = self.alloc_remote(args, len(args))
        thread = self.kernel32.CreateRemoteThread(self.handle, None, None, c_long(function_addr),
                                                  c_long(args_addr), None, None)
        if not thread:
            raise WinError()
        if self.kernel32.WaitForSingleObject(thread, 0xFFFFFFFF) == 0xFFFFFFFF:
            raise WinError()
        ret = self.read_memory(args_addr, len(args))
        if not self.kernel32.GetExitCodeThread(thread, byref(dll_addr)):
            raise WinError()
        self.free_remote(args_addr, len(args))
        return ret

    def read_memory(self, addr, size):
        buffer = create_string_buffer(size)
        if not self.kernel32.ReadProcessMemory(self.handle, c_long(addr), buffer, size, None):
            raise WinError()
        return buffer

    def write_memory(self, addr, string):
        size = len(string)
        if not self.kernel32.WriteProcessMemory(self.handle, addr, string, size, None):
            raise WinError()

    def load_library(self, buffer):
        path_len = self.kernel32.GetSystemWow64DirectoryW(None, 0)
        wow64dir = create_unicode_buffer(path_len + 1)
        self.kernel32.GetSystemWow64DirectoryW(byref(wow64dir), path_len)
        
        module = wow64dir.value + '\\kernel32.dll'
        
        function_addr = self.get_address_from_module(module, "LoadLibraryA")
        dll_addr = self.create_remote_thread(function_addr, buffer)
        return dll_addr

    def inject_dll(self, path):
        return self.load_library(path.encode("ascii"))

    def call_from_injected(self, path, dll_addr, function, args):
        function_offset = self.get_offset_of_exported_function(path, function)
        return self.read_remote_thread(dll_addr + function_offset, args)

    def get_offset_of_exported_function(self, module, function):
        base_addr = self.get_process_module_base(module.encode('ascii'))
        function_addr = self.get_address_from_module(module, function)
        return function_addr - int.from_bytes(base_addr, "little")