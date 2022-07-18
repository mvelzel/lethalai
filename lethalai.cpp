#include "lethalai.h"
#include "helpers/logger.h"
#include "helpers/code_injector.h"
#include "environment/input_handler.h"
#include "environment/state_observer.h"
#include "environment/event_observer.h"
#include "environment/game_state.h"
#include "gym/lethal_gym.h"
#include "rl/main_dqn.h"
#include <iostream>
#include <array>
#include <tuple>
#include <vector>
#include <Windows.h>

int global_counter = 0;

environment::InputHandler *input_handler = NULL;
environment::StateObserver *state_observer = NULL;

void HandleState(environment::GameState *game_state) {
  std::cout << *game_state->GetBall() << std::endl;

  if (input_handler != NULL) {
    if (global_counter % 100 == 0) {
      std::vector<environment::InputAction> actions{ environment::kSwing };
      input_handler->InputActions(1, actions);
      input_handler->InputActions(2, actions);
      input_handler->InputActions(1, std::vector<environment::InputAction>());
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

void HandleEvent(events::Event *event, unsigned int context) {
  std::cout << "Event triggered, eventnum: " << event->GetId() << ", " <<
    "context: " << std::hex << context << std::endl;

  environment::Player** players = state_observer->GetGameState()->GetPlayers();
  for (int i = 0; i < 4; i++) {
    if (players[i] != NULL) {
      unsigned int player_base = players[i]->GetPlayerBase();
      std::cout << "Player " << i + 1 << " Base: " << std::hex << player_base << std::endl;
    }
  }
  std::cout << std::endl;
}

void WINAPI InitDLL() {
  //helpers::Logger *logger = new helpers::Logger("./lethalai_log.txt");
  //logger->WriteLine("Attached DLL");

  helpers::CodeInjector::CreateConsole();

  std::cout << "Injected DLL" << std::endl;

  //input_handler = new environment::InputHandler(true);
  //input_handler->DisableOverride(1);
  //input_handler->Inject();
  //state_observer = new environment::StateObserver(true);
  //environment::EventObserver *event_observer = new environment::EventObserver(true);


  //state_observer->SetObserver(HandleState);
  //event_observer->SetObserver(HandleEvent);
  //
  helpers::CodeInjector::SpeedHack(10.0f);
  //gym::LethalGym* lethal_gym = new gym::LethalGym();

  //while (true) {
  //    // TODO deadlocks when runs too fast
  //    std::vector<environment::InputAction> action{ environment::kSwing };
  //    std::vector<std::vector<environment::InputAction>> actions{ action, action };
  //    auto result = lethal_gym->Step(actions);
  //    environment::GameState* observation = std::get<0>(result);
  //    float reward = std::get<1>(result);
  //    std::vector<float> normalized = observation->NormalizeFloats(0);
  //    std::cout << "GameState: [";
  //    for (auto i: normalized)
  //          std::cout << i << ", ";
  //    std::cout << "]" << std::endl;
  //    std::cout << "Reward: " << reward << std::endl;
  //}

  //rl::MainDqn* main_dqn = new rl::MainDqn();
  //main_dqn->Train();
}

BOOL APIENTRY DllMain (HMODULE h_module,
    DWORD ul_reason_for_call,
    LPVOID lp_reserved) {
  if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitDLL, NULL, 0, NULL);
  }
  return TRUE; 
}

struct TestFunctionStruct {
  int input;
  std::array<float, 4> output;
};

void TestFunction(void* args) {
  struct TestFunctionStruct *arg = (struct TestFunctionStruct*) args;
  std::cout << "Lmao Test: " << arg->input << std::endl;
  std::cout << "Lmao Test2: ";
  for (const float &flt : arg->output)
    std::cout << flt << ",";
  std::cout << std::endl;

  arg->output = { 1.0f, 2.6f, 2.2f, 48.023f };
}

gym::LethalGym* global_gym;

void ExportInit(void* args) {
  global_gym = new gym::LethalGym();
}

struct ResetStruct {
  std::array<float, environment::GameState::kStateSize> state;
};

void ExportReset(void* args) {
  struct ResetStruct *arg = (struct ResetStruct*) args;
  environment::GameState *state = global_gym->Reset();
  arg->state = state->NormalizeFloats(0);
}

struct StepStruct {
  int action;
  std::array<float, environment::GameState::kStateSize> state;
  float reward;
  bool done;
};

void ExportStep(void* args) {
  struct StepStruct *arg = (struct StepStruct*) args;
  std::vector<environment::InputAction> action { static_cast<environment::InputAction>(arg->action) };
  std::vector<std::vector<environment::InputAction>> actions { action };
  auto res = global_gym->Step(actions);
  arg->state = std::get<0>(res)->NormalizeFloats(0);
  arg->reward = *std::get<1>(res);
  arg->done = std::get<2>(res);;
}
