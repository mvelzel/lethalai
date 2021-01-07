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

    DWORD player_spawn_return = 0;

    DWORD player_spawn_object = 0;

    __declspec(naked) void PlayerSpawnInject(void) {
        __asm {
            pop player_spawn_return

            mov ecx, esi

            mov player_spawn_object, ecx

            call edx
            lea ecx, [ebp-0x08]

            push player_spawn_return
            ret
        }
    }

    DWORD spawn_player_call = 0x0;

    DWORD WINAPI SpawnPlayerRemote(LPVOID in) {
        DWORD p_base = *static_cast<DWORD*>(in);
        __asm {
            push ebx
            push ecx

            mov ecx, player_spawn_object
            mov ebx, p_base
            add ebx, 0x1bc
            push ebx
            call spawn_player_call

            pop ecx
            pop ebx
        }
        //delete &p_base;
    }

    DWORD spawn_ball_call = 0x0;

    DWORD WINAPI SpawnBallRemote(LPVOID in) {
        DWORD* ball_position = static_cast<DWORD*>(in);
        __asm {
            push ecx
            
            push 0x00
            mov ecx, ball_position
            push ecx
            mov ecx, ball_base
            call spawn_ball_call

            pop ecx
        }
        delete ball_position;
    }

    DWORD ball_spawn_x = 0;
    DWORD ball_spawn_y = 0;

    DWORD ball_spawn_return = 0;

    __declspec(naked) void BallSpawnInject(void) {
        __asm {
            pop ball_spawn_return

            push ebx

            mov ebx, [eax]
            mov ball_spawn_x, ebx
            mov ebx, [eax+4]
            mov ball_spawn_y, ebx

            pop ebx

            push eax
            mov ecx,ebx
            call edx

            push ball_spawn_return
            ret
        }
    }


    DWORD routine_end_return = 0;
    DWORD ll_call_addr = 0x0;

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
    DWORD start_event_loop_jmp = 0x0;

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

        ll_call_addr = base_address + 0x145862;
        start_event_loop_jmp = base_address + 0xBAC35; 
        spawn_player_call = base_address + 0xA4A50;
        spawn_ball_call = base_address + 0x1FB40;

        DWORD start_event_loop_address = base_address + 0xBA97C;
        helpers::CodeInjector::CodeCave(start_event_loop_address, StartEventLoop, 3);

        DWORD ball_inject_address = base_address + 0x102AC3;
        helpers::CodeInjector::CodeCave(ball_inject_address, BallInject, 0);

        DWORD routine_end_inject_address = base_address + 0x102C9F;
        helpers::CodeInjector::CodeCave(routine_end_inject_address, RoutineEndInject, 0);

        DWORD player_inject_address = base_address + 0x100BA5;
        helpers::CodeInjector::CodeCave(player_inject_address, PlayerInject, 0);

        DWORD player_spawn_inject_address = base_address + 0xA35FE;
        helpers::CodeInjector::CodeCave(player_spawn_inject_address, PlayerSpawnInject, 2);

        DWORD ball_spawn_inject_address = base_address + 0xA372E;
        helpers::CodeInjector::CodeCave(ball_spawn_inject_address, BallSpawnInject, 0);
    }

    void StateObserver::UpdateState() {
        if (main_routine_active && event_counter == 0) {
            event_counter = 1;
            //std::cout << "Main routine active" << std::endl;
            //std::cout << "Ballbase is: 0x" << std::hex << ball_base << std::endl;
            if (ball_base) {
                int x_coord = *(int*)(*(DWORD*)(ball_base + 0x14) + 0x18);
                int y_coord = *(int*)(*(DWORD*)(ball_base + 0x14) + 0x1C);
                int x_velocity = *(int*)(*(DWORD*)(ball_base + 0x194) + 0x10);
                int y_velocity = *(int*)(*(DWORD*)(ball_base + 0x194) + 0x14);
                int ball_speed = *(int*)(*(DWORD*)(ball_base + 0x194) + 0x12C);
                int ball_tag = *(int*)(*(DWORD*)(ball_base + 0x194) + 0x130);
                int hitstun_countdown = *(int*)(*(DWORD*)(ball_base + 0x194) + 0x128);

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
                    int x_coord = *(int*)(*(DWORD*)(player_bases[i] + 0x14) + 0x18);
                    int y_coord = *(int*)(*(DWORD*)(player_bases[i] + 0x14) + 0x1C);
                    bool facing_direction = *(bool*)(*(DWORD*)(player_bases[i] + 0x194) + 0x4);
                    int x_velocity = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x10);
                    int y_velocity = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x14);
                    int character_state = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x134);
                    int animation_state = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x138);
                    int hitstun_countdown = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x13c);
                    int special_meter = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x150);
                    int stocks = *(int*)(*(DWORD*)(player_bases[i] + 0x194) + 0x178);

                    players[i]->SetXCoord(x_coord);
                    players[i]->SetYCoord(y_coord);
                    players[i]->SetFacingDirection(facing_direction);
                    players[i]->SetXVelocity(x_velocity);
                    players[i]->SetYVelocity(y_velocity);
                    players[i]->SetCharacterState(character_state);
                    players[i]->SetAnimationState(animation_state);
                    players[i]->SetHitstunCountdown(hitstun_countdown);
                    players[i]->SetSpecialMeter(special_meter);
                    players[i]->SetPlayerBase(player_bases[i]);
                    players[i]->SetStocks(stocks);
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

    GameState* StateObserver::GetGameState() {
        return this->game_state;
    }

    void StateObserver::SpawnBall() {
        if (ball_spawn_x != 0 && ball_spawn_x != 0)
            this->SpawnBall(ball_spawn_x, ball_spawn_y);
    }

    void StateObserver::SpawnBall(DWORD x, DWORD y) {
        if (ball_base == 0)
            return;
        DWORD* ball_position = new DWORD[2];
        ball_position[0] = x;
        ball_position[1] = y;
        CreateRemoteThread(GetCurrentProcess(), NULL, 0,
                (LPTHREAD_START_ROUTINE) SpawnBallRemote,
                ball_position, NULL, NULL);
    }

    void StateObserver::SpawnPlayer(int player_number) {
        if (player_number < 0)
            return;
        Player* player = this->game_state->GetPlayers()[player_number];
        if (player == NULL)
            return;

        DWORD* player_base = new DWORD(player->GetPlayerBase());
        if (player_base == NULL || *player_base == 0)
            return;
        CreateRemoteThread(GetCurrentProcess(), NULL, 0,
                (LPTHREAD_START_ROUTINE) SpawnPlayerRemote,
                player_base, NULL, NULL);
    }

    void StateObserver::HealPlayer(int player_number, int amount) {
        if (player_number < 0)
            return;
        Player* player = this->game_state->GetPlayers()[player_number];
        if (player == NULL)
            return;

        int *stocks = (int*)(*(DWORD*)(player->GetPlayerBase() + 0x194) + 0x178);
        *stocks = amount;
    }

    void StateObserver::HealPlayers(int amount) {
        for (int i = 0; i < 4; i++) {
            this->HealPlayer(i, amount);
        }
    }

    void StateObserver::SpawnPlayers() {
        for (int i = 0; i < 4; i++) {
            this->SpawnPlayer(i);
        }
    }
}
