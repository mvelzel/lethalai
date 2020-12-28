#ifndef CODE_INJECTOR_H
#define CODE_INJECTOR_H

#include <Windows.h>

namespace helpers {
    class CodeInjector {
        public:
            static void WriteBytesASM(DWORD dest_address,
                    LPVOID patch,
                    DWORD num_bytes);
            static void CreateConsole();
            static void CodeCave(DWORD dest_address,
                    VOID (*func)(VOID),
                    BYTE nop_count);
            static void SpeedHack(float speed);
    };
}

#endif
