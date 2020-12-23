#include "lethalai.h"
#include "helpers/logger.h"
#include "helpers/code_injector.h"
#include <Windows.h>

BOOL APIENTRY DllMain (HMODULE h_module,
        DWORD ul_reason_for_call,
        LPVOID lp_reserved) {
    switch(ul_reason_for_call) { 
        case DLL_PROCESS_ATTACH: {
                helpers::Logger *logger = new helpers::Logger("./lethalai_log.txt");
                logger->WriteLine("Attached DLL");

                helpers::CodeInjector::CreateConsole();

                printf("Injected DLL");
            }
            break;

        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
            // Perform any necessary cleanup.
            break;
    }
    return TRUE; 
}
