#include "code_injector.h"
#include <Windows.h>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <iostream>

namespace helpers {
    float game_speed;

	void CodeInjector::WriteBytesASM(DWORD dest_address,
			LPVOID patch,
			DWORD num_bytes) {
		// Store old protection of the memory page
		DWORD old_protect = 0;

		// Store the source address
		DWORD src_address = PtrToUlong(patch);

		// Make sure page is writeable
		VirtualProtect((void*)(dest_address), num_bytes, PAGE_EXECUTE_READWRITE, &old_protect);

		// Do the patch (oldschool style to avoid memcpy)
		__asm
		{
			nop      // Filler
				nop      // Filler
				nop      // Filler

				mov esi, src_address  // Save the address
				mov edi, dest_address // Save the destination address
				mov ecx, num_bytes  // Save the size of the patch
				Start:
				cmp ecx, 0    // Are we done yet?
				jz Exit     // If so, go to end of function

				mov al, [esi]   // Move the byte at the patch into AL
				mov [edi], al   // Move AL into the destination byte
				dec ecx     // 1 less byte to patch
				inc esi     // Next source byte
				inc edi     // Next destination byte
				jmp Start    // Repeat the process
				Exit:
				nop      // Filler
				nop      // Filler
				nop      // Filler
		}

		// Restore old page protection
		VirtualProtect((void*)(dest_address), num_bytes, old_protect, &old_protect);
	}

	void CodeInjector::CreateConsole() {
		int hConHandle = 0;
		HANDLE lStdHandle = 0;
		FILE *fp = 0;

		// Allocate a console
		AllocConsole();

        freopen_s(&fp, "CONOUT$", "w", stdout);
		// redirect unbuffered STDOUT to the console
		//lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		//hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
		//fp = _fdopen(hConHandle, "w");
		//*stdout = *fp;
		//setvbuf(stdout, NULL, _IONBF, 0);

		//// redirect unbuffered STDIN to the console
		//lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
		//hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
		//fp = _fdopen(hConHandle, "r");
		//*stdin = *fp;
		//setvbuf(stdin, NULL, _IONBF, 0);

		//// redirect unbuffered STDERR to the console
		//lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
		//hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
		//fp = _fdopen(hConHandle, "w");
		//*stderr = *fp;
		//setvbuf(stderr, NULL, _IONBF, 0); 
	}

	void CodeInjector::CodeCave(DWORD dest_address,
			void (*func)(),
			BYTE nop_count) {
		// Calculate the code cave for chat interception
		DWORD offset = (PtrToUlong(func) - dest_address) - 5;

		// Buffer of NOPs, static since we limit to 'UCHAR_MAX' NOPs
		BYTE nop_patch[0xFF] = {0};

		// Construct the patch to the function call
		BYTE patch[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};
		memcpy(patch + 1, &offset, sizeof(DWORD));
		WriteBytesASM(dest_address, patch, 5);

		// We are done if we do not have NOPs
		if(nop_count == 0)
			return;

		// Fill it with nops
		memset(nop_patch, 0x90, nop_count);

		// Make the patch now
		WriteBytesASM(dest_address + 5, nop_patch, nop_count); 
	}

    void CodeInjector::SpeedHack(float speed) {
        LPVOID initialize_speedhack = (LPVOID)GetProcAddress(
                GetModuleHandleA("speedhack-i386.dll"),
                "InitializeSpeedhack"); 
        if (initialize_speedhack != 0) {
            std::cout << "Found InitializeSpeedhack at: " << initialize_speedhack << std::endl;
            game_speed = speed;
            HANDLE current_process = GetCurrentProcess();

            union f2u {
                float f;
                uint32_t u;
            };
            f2u f = {game_speed};
            HANDLE thread = CreateRemoteThread(
                    GetCurrentProcess(), NULL, NULL, 
                    (LPTHREAD_START_ROUTINE) initialize_speedhack,
                    (LPVOID)f.u, NULL, NULL);


            if (thread) {
                std::cout << "Called remote thread: " << thread << std::endl;
                CloseHandle(thread);
            } else {
                std::cout << "Creating remote thread failed." << std::endl;
            }
        } else {

            std::cout << "Cannot load speedhack, speedhack-i386.dll not found." << std::endl;
        }
    }
}

