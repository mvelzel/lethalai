#pragma once

#include <vector>
#include <deque>
#include "../environment/game_state.h"
#include "../environment/input_handler.h"
#include "../gym/lethal_gym.h"
#include "agent.h"

namespace rl {
    class MultiAgent {
        private:
            int save_steps;
            int team_change;
            int swap_steps;
            int window;
            float play_against_latest_model_ratio;

            int save_steps_counter;
            int team_change_counter;
            int swap_steps_counter;
            int active_agent;

            std::deque<std::string> policy_deque;

            Agent *agent1;
            Agent *agent2;

            gym::LethalGym* gym;

            bool is_demonstration;

            void SavePolicy();
            void SwapPolicy();
            void ChangeTeams();
        public:
            MultiAgent(int save_steps, int team_change, int swap_steps,
                    int window, float play_against_latest_model_ratio, gym::LethalGym *gym,
                    bool continue_training=false, bool is_demonstration=false);

            std::vector<std::vector<environment::InputAction>> ChooseActions(
                    environment::GameState* observation);
            void StoreTransitions(environment::GameState* observation,
                    std::vector<std::vector<environment::InputAction>> actions,
                    std::vector<float> rewards, 
                    environment::GameState* new_observation, bool done);
            void Learn();
    };
}
