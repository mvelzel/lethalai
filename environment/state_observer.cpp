#include "state_observer.h"
#include <Windows.h>
#include <sstream>
#include <string>
#include <iostream>
#include "../helpers/code_injector.h"

namespace environment {
    environment::StateObserver *global_observer = NULL;

    DWORD main_routine_active = 0;

    DWORD ball_return = 0;
    DWORD ball_base = 0;

    __declspec(naked) void BallInject(void) {
        __asm {
            pop ball_return

            mov eax, [ecx]
            mov edx, [eax+0x40]

            mov ball_base, ecx

            push ball_return
            ret
        }
    }


    DWORD player_bases[4] = { 0, 0, 0, 0 };
    DWORD player_return = 0;
    DWORD current_player = 0;

    __declspec(naked) void PlayerInject(void) {
        __asm {
            pop player_return

            mov edx,[esi]
            mov edx,[edx + 0x4C]
            
            push eax
            push ebx
            push ecx

            mov eax, current_player

            cmp eax, 4
            jb player_start
            mov player_bases[TYPE player_bases * 0], 0
            mov player_bases[TYPE player_bases * 1], 0
            mov player_bases[TYPE player_bases * 2], 0
            mov player_bases[TYPE player_bases * 3], 0
            mov eax, 0
            mov current_player, eax

        player_start:
            cmp player_bases[0], esi
            jne player_not_one

            mov eax, 0
            mov current_player, eax

        player_not_one:
            imul eax, TYPE player_bases
            mov player_bases[eax], esi
            mov eax, current_player
            inc eax
            mov current_player, eax
            pop ecx
            pop ebx
            pop eax

            push player_return
            ret
        }
    }


    DWORD routine_end_return = 0;
    DWORD ll_call_addr = 0x01035862;

    DWORD event_counter = 0;

    __declspec(naked) void RoutineEndInject(void) {
        __asm {
            pop routine_end_return

            call ll_call_addr 

            mov main_routine_active, 1
            mov event_counter, 0

            push routine_end_return
            ret
        }
    }

    DWORD start_event_loop_return = 0;
    DWORD start_event_loop_jmp = 0x00FAAC35;

    __declspec(naked) void StartEventLoop(void) {
        __asm {
            pop start_event_loop_return

            pushad
            pushfd
        }

        global_observer->UpdateState();

        __asm {
            popfd
            popad

            test al,al
            je start_event_loop_jump

            push start_event_loop_return
            ret

        start_event_loop_jump:
            jmp start_event_loop_jmp
        }
    }


    StateObserver::StateObserver(bool enabled) {
        this->game_state = new GameState();
        this->observer = NULL;
        if (enabled) {
            this->Enable();
            global_observer = this;
        }
    }

    void StateObserver::Enable() {
        DWORD base_address = (DWORD)GetModuleHandleA("lethalleague.exe");

        DWORD start_event_loop_address = base_address + 0xBA97C;
        helpers::CodeInjector::CodeCave(start_event_loop_address, StartEventLoop, 3);

        DWORD ball_inject_address = base_address + 0x102AC3;
        helpers::CodeInjector::CodeCave(ball_inject_address, BallInject, 0);

        DWORD routine_end_inject_address = base_address + 0x102C9F;
        helpers::CodeInjector::CodeCave(routine_end_inject_address, RoutineEndInject, 0);

        DWORD player_inject_address = base_address + 0x100BA5;
        helpers::CodeInjector::CodeCave(player_inject_address, PlayerInject, 0);
    }

    void StateObserver::UpdateState() {
        if (main_routine_active && event_counter == 0) {
            event_counter = 1;
            //std::cout << "Main routine active" << std::endl;
            //std::cout << "Ballbase is: 0x" << std::hex << ball_base << std::endl;
            if (ball_base) {
                unsigned int x_coord = *(unsigned int*)(*(DWORD*)(ball_base + 0x14) + 0x18);
                unsigned int y_coord = *(unsigned int*)(*(DWORD*)(ball_base + 0x14) + 0x1C);
                unsigned int x_velocity = *(unsigned int*)(*(DWORD*)(ball_base + 0x194) + 0x10);
                unsigned int y_velocity = *(unsigned int*)(*(DWORD*)(ball_base + 0x194) + 0x14);
                unsigned int ball_speed = *(unsigned int*)(*(DWORD*)(ball_base + 0x194) + 0x12C);
                unsigned int ball_tag = *(unsigned int*)(*(DWORD*)(ball_base + 0x194) + 0x130);
                signed int hitstun_countdown = *(signed int*)(*(DWORD*)(ball_base + 0x194) + 0x128);

                Ball *ball = this->game_state->GetBall();
                ball->SetXCoord(x_coord);
                ball->SetYCoord(y_coord);
                ball->SetXVelocity(x_velocity);
                ball->SetYVelocity(y_velocity);
                ball->SetBallSpeed(ball_speed);
                ball->SetBallTag(ball_tag);
                ball->SetHitstunCountdown(hitstun_countdown);

                //std::cout << *ball << std::endl;
            }
            Player** players = this->game_state->GetPlayers();
            for (int i = 0; i < 4; i++) {
                if (player_bases[i] != 0) {
                    unsigned int x_coord = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x14) + 0x18);
                    unsigned int y_coord = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x14) + 0x1C);
                    bool facing_direction = *(bool*)(*(DWORD*)(player_bases[i] + 0x194) + 0x4);
                    unsigned int x_velocity = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x10);
                    unsigned int y_velocity = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x14);
                    unsigned int character_state = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x134);
                    unsigned int animation_state = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x138);
                    signed int hitstun_countdown = *(signed int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x13c);
                    unsigned int special_meter = *(unsigned int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x150);

                    players[i]->SetXCoord(x_coord);
                    players[i]->SetYCoord(y_coord);
                    players[i]->SetFacingDirection(facing_direction);
                    players[i]->SetXVelocity(x_velocity);
                    players[i]->SetYVelocity(y_velocity);
                    players[i]->SetCharacterState(character_state);
                    players[i]->SetAnimationState(animation_state);
                    players[i]->SetHitstunCountdown(hitstun_countdown);
                    players[i]->SetSpecialMeter(special_meter);
                    //std::cout << *players[i] << std::endl;
                } else {
                    if (players[i] != NULL)
                        delete players[i];
                    players[i] = NULL;
                }
            }
            //std::cout << std::endl;
            current_player = 4;
            if (this->observer != NULL)
                this->observer(this->game_state);
        }
    }

    void StateObserver::SetObserver(void (*observer)(GameState *)) {
        this->observer = observer;
    }
}
