#include "input_handler.h"
#include "../helpers/code_injector.h"
#include <iostream>
#include <Windows.h>

namespace environment {
    DWORD override_p1_enabled = 0;
    DWORD override_p2_enabled = 0;
    DWORD override_p3_enabled = 0;
    DWORD override_p4_enabled = 0;


    DWORD player_pressed_objects[4] = { 0, 0, 0, 0 };

    DWORD player_objects_hook_return = 0;

    __declspec(naked) void PlayerObjectsHook(void) {
        __asm {
            pop player_objects_hook_return

            cmp edx, 3
            ja skip_set_values

            mov player_pressed_objects[TYPE player_pressed_objects * edx], esi
            
        skip_set_values:
            push esi
            mov ecx,ebx
            mov [ebp-0x18],edi

            push player_objects_hook_return
            ret
        }
    }

    DWORD key_pressed_return = 0;
    DWORD key_pressed_jmp = 0x00FB14D0;

    int actions_pressed_p1[8] = { 9, 9, 9, 9, 9, 9, 9, 9 };
    int actions_pressed_p2[8] = { 9, 9, 9, 9, 9, 9, 9, 9 };
    int actions_pressed_p3[8] = { 9, 9, 9, 9, 9, 9, 9, 9 };
    int actions_pressed_p4[8] = { 9, 9, 9, 9, 9, 9, 9, 9 };

    __declspec(naked) void KeyPressedInject(void) {
        _asm {
            pop key_pressed_return
            push ebx
            push ecx

            mov ecx, 0
            mov ebx, [esp+8]
        input_loop:
            cmp player_pressed_objects[TYPE player_pressed_objects * 0], esi
            je p1action
            cmp player_pressed_objects[TYPE player_pressed_objects * 1], esi
            je p2action
            cmp player_pressed_objects[TYPE player_pressed_objects * 2], esi
            je p3action
            cmp player_pressed_objects[TYPE player_pressed_objects * 3], esi
            je p4action
        p1action:
            cmp override_p1_enabled, 1
            jne end_loop

            mov eax, 1
            cmp actions_pressed_p1[TYPE actions_pressed_p1 * ecx], ebx
            je end_loop
            jmp continue_loop
        p2action:
            cmp override_p2_enabled, 1
            jne end_loop

            mov eax, 1
            cmp actions_pressed_p2[TYPE actions_pressed_p2 * ecx], ebx
            je end_loop
            jmp continue_loop
        p3action:
            cmp override_p3_enabled, 1
            jne end_loop

            mov eax, 1
            cmp actions_pressed_p3[TYPE actions_pressed_p3 * ecx], ebx
            je end_loop
            jmp continue_loop
        p4action:
            cmp override_p4_enabled, 1
            jne end_loop

            mov eax, 1
            cmp actions_pressed_p4[TYPE actions_pressed_p4 * ecx], ebx
            je end_loop
            jmp continue_loop

        continue_loop:
            mov eax, 0
            inc ecx
            cmp ecx, 8            
            jb input_loop
        end_loop:
            pop ecx
            pop ebx
            
            test al, al
            je jump

            push key_pressed_return
            ret
        jump:
            jmp key_pressed_jmp
        }
    }
    

    DWORD input_event_return = 0;
    DWORD input_event_call = 0x00FB0E80;

    DWORD input_event_value = 0x4B;
    DWORD input_event_object = 0x02E6B4C8;
    DWORD input_event_this = 0x128DEEE0;

    DWORD skip_override = 0;

    DWORD WINAPI InputEvent(LPVOID in) {
        int input_event = *static_cast<int*>(in);
        __asm {
            push eax
            push ecx
            push edx

            mov skip_override, 1

            mov eax, input_event
            mov edx, input_event_object
            mov ecx, input_event_this
            push edx
            push eax
            call input_event_call

            mov skip_override, 0

            pop edx
            pop ecx
            pop eax
        }
    }

    DWORD input_event_values_return = 0;

    void __declspec(naked) InputEventValues() {
        __asm {
            pop input_event_values_return
            push eax

            mov eax, [ebp-0x9C]
            mov input_event_value, eax
            mov eax, [esi+0xA8]
            mov input_event_this, eax

            pop eax
            movzx ecx,byte ptr [eax+0x00FAAC78]

            push input_event_values_return
            ret
        }
    }

    DWORD input_event_override_return = 0;

    //TODO make work for all players
    __declspec(naked) void InputEventOverride() {
        __asm {
            pop input_event_override_return

            cmp skip_override, 1
            je original

            //TODO make work for all players
            cmp eax, 0x16
            jb p2check
            cmp override_p1_enabled, 1
            jne original
            ret 8
        p2check:
            cmp override_p2_enabled, 1
            jne original
            ret 8

        original:
            push ebp
            mov ebp,esp
            lea eax,[ebp+8]

            push input_event_override_return
            ret
        }
    }

    DWORD end_event_loop_return = 0;
    DWORD end_event_loop_jmp = 0x00FAAC35;

    void PrintEndEventLoop() {
        printf("Event Loop End\n");
    }

    __declspec(naked) void EndEventLoop(void) {
        __asm {
            pop end_event_loop_return

            pushad
            pushfd
        }

        PrintEndEventLoop();

        __asm {
            popfd
            popad

            test al,al
            je event_loop_jump

            push end_event_loop_return
            ret

        event_loop_jump:
            jmp end_event_loop_jmp
        }
    }


    InputHandler::InputHandler(bool enabled) {
        if (enabled) {
            this->EnableOverride(1);
            this->EnableOverride(2);
            this->EnableOverride(3);
            this->EnableOverride(4);
        } else {
            this->DisableOverride(1);
            this->DisableOverride(2);
            this->DisableOverride(3);
            this->DisableOverride(4);
        }
    }

    void InputHandler::EnableOverride(int player_number) {
        switch (player_number) {
            case 1: this->override_p1 = true;
                    override_p1_enabled = 1;
                    return;
            case 2: this->override_p2 = true;
                    override_p2_enabled = 1;
                    return;
            case 3: this->override_p3 = true;
                    override_p3_enabled = 1;
                    return;
            case 4: this->override_p4 = true;
                    override_p4_enabled = 1;
                    return;
            default:
                    return;
        }
    }

    void InputHandler::DisableOverride(int player_number) {
        switch (player_number) {
            case 1: this->override_p1 = false;
                    override_p1_enabled = 0;
                    return;
            case 2: this->override_p2 = false;
                    override_p2_enabled = 0;
                    return;
            case 3: this->override_p3 = false;
                    override_p3_enabled = 0;
                    return;
            case 4: this->override_p4 = false;
                    override_p4_enabled = 0;
                    return;
            default:
                    return;
        }
    }

    void InputHandler::Inject() {
        DWORD base_address = (DWORD)GetModuleHandleA("lethalleague.exe");

        DWORD key_pressed_inject_address = base_address + 0xC13AE;
        helpers::CodeInjector::CodeCave(key_pressed_inject_address, KeyPressedInject, 3);

        DWORD input_event_values_address = base_address + 0xBA9AD;
        helpers::CodeInjector::CodeCave(input_event_values_address, InputEventValues, 2);

        DWORD player_objects_hook_address = base_address + 0xC1A7E;
        helpers::CodeInjector::CodeCave(player_objects_hook_address, PlayerObjectsHook, 1);

        DWORD input_event_override_address = base_address + 0xC0E80;
        helpers::CodeInjector::CodeCave(input_event_override_address, InputEventOverride, 1);

        //DWORD end_event_loop_address = base_address + 0xBA97C;
        //helpers::CodeInjector::CodeCave(end_event_loop_address, EndEventLoop, 3);
    }

    void InputHandler::InputActions(int player_number, InputAction *actions, int action_count) {
        if (input_event_values_return == 0 || player_objects_hook_return == 0)
            return;
        switch (player_number) {
            case 1: if (!this->override_p1) return;
            case 2: if (!this->override_p2) return;
            case 3: if (!this->override_p3) return;
            case 4: if (!this->override_p4) return;
        }
        HWND handle = FindWindow(NULL, "Lethal League");
        SendMessage(handle, WM_ACTIVATE, WA_CLICKACTIVE, NULL);

        for (int i = 0; i < 8; i++)
            switch (player_number) {
                case 1: actions_pressed_p1[i] = 9; break;
                case 2: actions_pressed_p2[i] = 9; break;
                case 3: actions_pressed_p3[i] = 9; break;
                case 4: actions_pressed_p4[i] = 9; break;
                default: return;
            }
            
        for (int i = 0; i < action_count; i++) {
            int key_press = InputHandler::ActionToKeyPress(actions[i]);
            switch (player_number) {
                case 1: actions_pressed_p1[actions[i]] = key_press; break;
                case 2: actions_pressed_p2[actions[i]] = key_press; break;
                case 3: actions_pressed_p3[actions[i]] = key_press; break;
                case 4: actions_pressed_p4[actions[i]] = key_press; break;
                default: return;
            }

            int *input_event = new int(InputHandler::ActionToEvent(player_number, actions[i]));
            //CreateRemoteThread(GetCurrentProcess(), NULL, 0,
            //        (LPTHREAD_START_ROUTINE) InputEvent,
            //        input_event, NULL, NULL);
            InputEvent(input_event);
        }

    }

    int InputHandler::ActionToKeyPress(InputAction action) {
        switch (action) {
            case kSwing: return 4;
            case kBunt: return 5;
            case kUp: return 0;
            case kDown: return 2;
            case kLeft: return 3;
            case kRight: return 1;
            case kJump: return 6;
            case kTaunt: return 8;
            default: return 9;
        }
    }

    int InputHandler::ActionToEvent(int player_number, InputAction action) {
        switch (player_number) {
            case 1:
                switch (action) {
                    case kSwing: return 0x19;
                    case kBunt: return 0x17;
                    case kUp: return 0x49;
                    case kDown: return 0x4A;
                    case kLeft: return 0x47;
                    case kRight: return 0x48;
                    case kJump: return 0x39;
                    case kTaunt: return 0x16;
                    default: return 0x4B;
                }
            case 2:
                switch (action) {
                    case kSwing: return 0x13;
                    case kBunt: return 0x12;
                    case kUp: return 0x10;
                    case kDown: return 0x4;
                    case kLeft: return 0x11;
                    case kRight: return 0x3;
                    case kJump: return 0x14;
                    case kTaunt: return 0xE;
                    default: return 0x4B;
                }
            default:
                return 0x4B;
        }
    }
}
