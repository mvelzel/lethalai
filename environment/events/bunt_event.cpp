#include "bunt_event.h"
#include <Windows.h>

#include "../../helpers/code_injector.h"

namespace events {
    events::Event *global_bunt_event = NULL;

    DWORD bunt_event_hook_return = 0;
    DWORD bunt_event_jmp = 0x0;

    DWORD bunt_player_value = 0x0;

    __declspec(naked) void BuntEventHook(void) {
        __asm {
            pop bunt_event_hook_return

            mov bunt_player_value, esi

            pushad
            pushfd
        }

        global_bunt_event->Trigger();

        __asm {
            popfd
            popad

            cmp eax,0x07
            je bunt_event_jump

            push bunt_event_hook_return
            ret

        bunt_event_jump:
            jmp bunt_event_jmp
        }
    }

    BuntEvent::BuntEvent(int id) : Event(id) {
        global_bunt_event = this;
        this->Inject();
    }

    void BuntEvent::Inject() {
        DWORD base_address = (DWORD)GetModuleHandleA("lethalleague.exe");

        bunt_event_jmp = base_address + 0xFBFA8;

        DWORD bunt_event_hook_address = base_address + 0xFBF9E;
        helpers::CodeInjector::CodeCave(bunt_event_hook_address, BuntEventHook, 0);
    }

    void BuntEvent::Trigger() {
        if (this->enabled && this->trigger_function != NULL) {
            this->trigger_function(this, bunt_player_value);
        }
    }
}
