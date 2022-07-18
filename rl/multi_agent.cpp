#include "multi_agent.h"
#include "./agents/actor_critic.h"
#include "./agents/dueling_dqn.h"
#include "action_translator.h"

namespace rl {
    MultiAgent::MultiAgent(int save_steps, int team_change, int swap_steps,
            int window, float play_against_latest_model_ratio, gym::LethalGym *gym,
            bool continue_training, bool is_demonstration) {
        this->gym = gym;
        this->is_demonstration = is_demonstration;
        this->save_steps = save_steps;
        this->team_change = team_change;
        this->swap_steps = swap_steps;
        this->window = window;
        this->play_against_latest_model_ratio = play_against_latest_model_ratio;

        this->save_steps_counter = 0;
        this->team_change_counter = 0;
        this->swap_steps_counter = 0;
        this->active_agent = 0;

        //this->agent1 = new ActorCriticAgent(0.99f, 0.00005f, 148, 32,
        //        ActionTranslator::total_action_count);
        //this->agent2 = new ActorCriticAgent(0.99f, 0.00005f, 148, 32,
        //        ActionTranslator::total_action_count);
        this->agent1 = new DuelingDeepQAgent(0.99f, 1.0f, 0.005f, 148, 32,
                ActionTranslator::total_action_count);
        this->agent2 = new DuelingDeepQAgent(0.99f, 1.0f, 0.005f, 148, 32,
                ActionTranslator::total_action_count);
        //this->agent1 = new DuelingDeepQAgent(0.99f, 1.0f, 0.001f, 148, 32,
        //        ActionTranslator::total_action_count);
        //this->agent2 = new DuelingDeepQAgent(0.99f, 1.0f, 0.001f, 148, 32,
        //        ActionTranslator::total_action_count);

        if (!continue_training) {
            this->SavePolicy();
            this->SwapPolicy();
        } else {
            for (int i = 0; i < window; i++) {
                this->policy_deque.push_back("policy" + std::to_string(i + 1));
            }
            this->SwapPolicy();
            this->ChangeTeams();
            this->SwapPolicy();
        }
    }

    std::vector<std::vector<environment::InputAction>> MultiAgent::ChooseActions(
            environment::GameState* observation) {
        std::vector<std::vector<environment::InputAction>> actions;

        std::array<float, environment::GameState::kStateSize> obs1 = observation->NormalizeFloats(0);
        std::array<float, environment::GameState::kStateSize> obs2 = observation->NormalizeFloats(1);
        std::vector<float> obs_agent1 { obs1.begin(), obs1.end() };
        std::vector<float> obs_agent2 { obs1.begin(), obs1.end() };

        std::vector<environment::InputAction> agent1_actions =
            ActionTranslator::TranslateAction(this->agent1->ChooseAction(obs_agent1));

        std::vector<environment::InputAction> agent2_actions =
            ActionTranslator::TranslateAction(this->agent2->ChooseAction(obs_agent2));

        actions.push_back(agent1_actions);
        actions.push_back(agent2_actions);

        return actions;
    }

    void MultiAgent::StoreTransitions(environment::GameState *observation,
            std::vector<std::vector<environment::InputAction>> actions,
            std::vector<float> rewards, 
            environment::GameState *new_observation, bool done) {
        if (this->is_demonstration)
            return;
        if (this->active_agent == 0) {
            std::array<float, environment::GameState::kStateSize> obs = observation->NormalizeFloats(0);
            std::array<float, environment::GameState::kStateSize> new_obs = observation->NormalizeFloats(0);
            std::vector<float> obs_agent1 { obs.begin(), obs.end() };
            std::vector<float> new_obs_agent1 { new_obs.begin(), new_obs.end() };
            this->agent1->StoreTransition(obs_agent1, actions[0][0], rewards[0], 
                    new_obs_agent1, done);
        } else if (this->active_agent == 1) {
            std::array<float, environment::GameState::kStateSize> obs = observation->NormalizeFloats(1);
            std::array<float, environment::GameState::kStateSize> new_obs = observation->NormalizeFloats(1);
            std::vector<float> obs_agent2 { obs.begin(), obs.end() };
            std::vector<float> new_obs_agent2 { new_obs.begin(), new_obs.end() };
            this->agent2->StoreTransition(obs_agent2, actions[1][0], rewards[1], 
                    new_obs_agent2, done);
        }
    }

    void MultiAgent::Learn() {
        if (this->is_demonstration)
            return;
        if (this->active_agent == 0) {
            this->agent1->Learn();
        } else if (this->active_agent == 1) {
            this->agent2->Learn();
        }

        this->save_steps_counter += 1;
        this->team_change_counter += 1;
        this->swap_steps_counter += 1;

        if (this->save_steps_counter == this->save_steps) {
            this->SavePolicy();
            this->save_steps_counter = 0;
        }
        if (this->swap_steps_counter == this->swap_steps) {
            this->SwapPolicy();
            this->swap_steps_counter = 0;
        }
        if (this->team_change_counter == this->team_change) {
            this->ChangeTeams();
            this->team_change_counter = 0;
        }
    }

    void MultiAgent::SavePolicy() {
        if (this->is_demonstration)
            return;
        int deque_size = this->policy_deque.size();
        if (deque_size < this->window) {
            std::string filename = "policy" + std::to_string(deque_size + 1);
            if (this->active_agent == 0) {
                this->agent1->StoreCheckpoint(filename);
            } else if (this->active_agent == 1) {
                this->agent2->StoreCheckpoint(filename);
            }
            this->policy_deque.push_front(filename);
        } else {
            std::string oldest_filename = this->policy_deque.back();
            this->policy_deque.pop_back();
            if (this->active_agent == 0) {
                this->agent1->StoreCheckpoint(oldest_filename);
            } else if (this->active_agent == 1) {
                this->agent2->StoreCheckpoint(oldest_filename);
            }
            this->policy_deque.push_front(oldest_filename);
        }
    }

    void MultiAgent::SwapPolicy() {
        if ((float) rand() / RAND_MAX > this->play_against_latest_model_ratio) {
            int num = (rand() % this->policy_deque.size()) + 1;
            if (this->active_agent == 0) {
                this->agent2->LoadCheckpoint("policy" + std::to_string(num));
            } else if (this->active_agent == 1) {
                this->agent1->LoadCheckpoint("policy" + std::to_string(num));
            }
        } else {
            if (this->active_agent == 0) {
                this->agent2->LoadCheckpoint(this->policy_deque.front());
            } else if (this->active_agent == 1) {
                this->agent1->LoadCheckpoint(this->policy_deque.front());
            }
        }
    }

    void MultiAgent::ChangeTeams() {
        if (this->active_agent == 0) {
            this->active_agent = 1;
            this->gym->info_list->SetLearningP1(false);
            this->gym->info_list->SetLearningP2(true);
            this->agent2->LoadCheckpoint(this->policy_deque.front());
            this->agent1->LoadCheckpoint(this->policy_deque.front());
        } else if (this->active_agent == 1) {
            this->active_agent = 0;
            this->gym->info_list->SetLearningP2(false);
            this->gym->info_list->SetLearningP1(true);
            this->agent1->LoadCheckpoint(this->policy_deque.front());
            this->agent2->LoadCheckpoint(this->policy_deque.front());
        }
    }
}

