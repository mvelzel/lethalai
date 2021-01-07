#include "death_event.h"
#include <Windows.h>

#include "../../helpers/code_injector.h"

namespace events {
    events::Event *global_death_event = NULL;

    DWORD death_event_hook_return = 0;

    DWORD death_player_value = 0x0;

    __declspec(naked) void DeathEventHook(void) {
        __asm {
            pop death_event_hook_return

            mov death_player_value, edi

            pushad
            pushfd
        }

        global_death_event->Trigger();

        __asm {
            popfd
            popad

            mov eax,[ebp+0x08]
            mov ecx,[esi+0x0000018C]

            push death_event_hook_return
            ret
        }
    }

    DeathEvent::DeathEvent(int id) : Event(id) {
        global_death_event = this;
        this->Inject();
    }

    void DeathEvent::Inject() {
        DWORD base_address = (DWORD)GetModuleHandleA("lethalleague.exe");

        DWORD death_event_hook_address = base_address + 0x250B4;
        helpers::CodeInjector::CodeCave(death_event_hook_address, DeathEventHook, 4);
    }

    void DeathEvent::Trigger() {
        if (this->enabled && this->trigger_function != NULL) {
            this->trigger_function(this, death_player_value);
        }
    }
}
