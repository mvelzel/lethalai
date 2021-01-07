#include "hit_event.h"
#include <Windows.h>

#include "../../helpers/code_injector.h"

namespace events {
    events::Event *global_hit_event = NULL;

    DWORD hit_event_hook_return = 0;

    DWORD hit_player_value = 0x0;

    __declspec(naked) void HitEventHook(void) {
        __asm {
            pop hit_event_hook_return

            mov hit_player_value, esi

            pushad
            pushfd
        }

        global_hit_event->Trigger();

        __asm {
            popfd
            popad

            mov eax,[esi+0x000002A0]

            push hit_event_hook_return
            ret
        }
    }

    HitEvent::HitEvent(int id) : Event(id) {
        global_hit_event = this;
        this->Inject();
    }

    void HitEvent::Inject() {
        DWORD base_address = (DWORD)GetModuleHandleA("lethalleague.exe");

        DWORD hit_event_hook_address = base_address + 0x100E04;
        helpers::CodeInjector::CodeCave(hit_event_hook_address, HitEventHook, 1);
    }

    void HitEvent::Trigger() {
        if (this->enabled && this->trigger_function != NULL) {
            this->trigger_function(this, hit_player_value);
        }
    }
}
