#include "lethalai.h"
#include "helpers/logger.h"
#include "helpers/code_injector.h"
#include "environment/input_handler.h"
#include "environment/state_observer.h"
#include <iostream>
#include <Windows.h>

int global_counter = 0;

environment::InputHandler *input_handler = NULL;

void HandleState(environment::GameState *game_state) {
    //std::cout << *game_state->GetBall() << std::endl;

    if (input_handler != NULL) {
        if (global_counter % 100 == 0) {
            environment::InputAction actions[1] = { environment::kSwing };
            input_handler->InputActions(1, actions, 1);
            input_handler->InputActions(2, actions, 1);
            input_handler->InputActions(1, NULL, 0);
            if (global_counter == 500) {
                input_handler->EnableOverride(1);
            } else if (global_counter == 1000) {
                input_handler->DisableOverride(1);
                global_counter = 0;
            }
        }
        global_counter++;
    }
}

void WINAPI Init() {
    helpers::Logger *logger = new helpers::Logger("./lethalai_log.txt");
    logger->WriteLine("Attached DLL");

    helpers::CodeInjector::CreateConsole();

    std::cout << "Injected DLL" << std::endl;

    input_handler = new environment::InputHandler(true);
    input_handler->DisableOverride(1);
    input_handler->Inject();
    environment::StateObserver *state_observer = new environment::StateObserver(true);

    helpers::CodeInjector::SpeedHack(10.0f);
    state_observer->SetObserver(HandleState);

    //int i = 1;
    //while (true) {
    //    Sleep(1500);

    //    if (i%10 == 0)
    //        input_handler->DisableOverride(1);
    //    if (i%20 == 0)
    //        input_handler->EnableOverride(1);
    //    environment::InputAction actions[1] = { environment::kSwing };
    //    input_handler->InputActions(1, actions, 1);
    //    input_handler->InputActions(2, actions, 1);
    //    input_handler->InputActions(1, NULL, 0);

    //    i++;
    //}
}

BOOL APIENTRY DllMain (HMODULE h_module,
        DWORD ul_reason_for_call,
        LPVOID lp_reserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Init, NULL, 0, NULL);
    }
    return TRUE; 
}
