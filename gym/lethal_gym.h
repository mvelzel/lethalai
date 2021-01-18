#ifndef LETHAL_GYM_H
#define LETHAL_GYM_H

#include <tuple>
#include <mutex>
#include <condition_variable>
#include "../environment/input_handler.h"
#include "../environment/game_state.h"
#include "../environment/state_observer.h"
#include "../environment/event_observer.h"
#include "../graphics/graphics_handler.h"

namespace gym {
    class LethalGym {
        private:
            environment::InputHandler* input_handler;
            environment::EventObserver* event_observer;
            environment::GameState* newest_observation = NULL;
            environment::StateObserver* state_observer = NULL;
            graphics::GraphicsHandler* graphics_handler = NULL;
            float newest_reward[2] = { 0.0f, 0.0f };
            int dying_player = -1;
            int steps_since_reward[2] = { 0, 0 };
            const int kMaxEmptySteps = 1000;
        public:
            bool game_notified = true;
            bool thread_notified = true;
            bool done = false;
            std::mutex mutex;
            std::condition_variable condition;
            std::vector<std::vector<environment::InputAction>> queued_actions;
            LethalGym();
            bool action_queued;
            std::tuple<environment::GameState*, float*, bool> Step(
                    std::vector<std::vector<environment::InputAction>> actions);
            environment::GameState* Reset();
            environment::InputHandler* GetInputHandler();
            environment::GameState* GetNewestObservation();
            void SetNewestObservation(environment::GameState* newest_observation);
            void CalculateReward(events::Event*, unsigned int context);

    };
}

#endif
