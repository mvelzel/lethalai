#include "lethal_gym.h"
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <tuple>
#include "../helpers/code_injector.h"

namespace gym {
    LethalGym* global_lethal_gym;

    void HandleEvent(events::Event* event, unsigned int context) {
        //std::cout << "Event triggered, eventnum: " << event->GetId() << ", " <<
        //    "context: " << std::hex << context << std::endl;
        global_lethal_gym->CalculateReward(event, context);
    }

    void HandleObservation(environment::GameState* game_state) {
        //std::cout << "Game Called" << std::endl;
        environment::Player** players = game_state->GetPlayers();
        for (int i = 0; i < 4; i++) {
            environment::Player* player = players[i];
            if (player != NULL) {
                if (!global_lethal_gym->done && player->GetCharacterState() == 18)
                    return;
            }
        }
        //std::cout << "New Observation" << std::endl;
        //Notify Step of new state
        {
            std::lock_guard<std::mutex> lk(global_lethal_gym->mutex);
            global_lethal_gym->SetNewestObservation(game_state);
        }
        global_lethal_gym->condition.notify_one();
        while (!global_lethal_gym->thread_notified)
            global_lethal_gym->condition.notify_one();
        

        //Wait for next step
        helpers::CodeInjector::SpeedHack(0.0f);
        //std::cout << "Game Waiting" << std::endl;
        {
            std::unique_lock<std::mutex> lk(global_lethal_gym->mutex);
            global_lethal_gym->thread_notified = false;
            global_lethal_gym->condition.wait(lk);
            global_lethal_gym->game_notified = true;
        }
        //std::cout << "Game Done Waiting" << std::endl;
        helpers::CodeInjector::SpeedHack(10.0f);
        if (global_lethal_gym->action_queued) {
            std::vector<std::vector<environment::InputAction>> actions =
                global_lethal_gym->queued_actions;

            global_lethal_gym->action_queued = false;

            for (unsigned int i = 0; i < actions.size(); i++) {
                global_lethal_gym->GetInputHandler()->InputActions(i + 1, actions[i]);
            }
        }
    }

    LethalGym::LethalGym() {
        global_lethal_gym = this;
        this->input_handler = new environment::InputHandler(true);
        //this->input_handler->DisableOverride(1);
        this->input_handler->Inject();
        this->state_observer = new environment::StateObserver(true);
        this->event_observer = new environment::EventObserver(true);
        this->event_observer->SetObserver(HandleEvent);
        this->state_observer->SetObserver(HandleObservation);
    }

    std::tuple<environment::GameState*, float, bool> LethalGym::Step(
            std::vector<std::vector<environment::InputAction>> actions) {
        //std::cout << "Step Called" << std::endl;
        bool d = this->done;
        if (d)
            return std::make_tuple(this->newest_observation,
                    this->newest_reward, d);
        {
            std::lock_guard<std::mutex> lk(this->mutex);
            this->queued_actions = actions;
            this->action_queued = true;
        }
        this->condition.notify_one();
        while (!this->game_notified)
            this->condition.notify_one();

        //std::cout << "Step Waiting" << std::endl;
        {
            std::unique_lock<std::mutex> lk(this->mutex);
            this->game_notified = false;
            this->condition.wait(lk);
            this->thread_notified = true;
        }
        //std::cout << "Step Done Waiting" << std::endl;

        float reward = this->newest_reward;
        if (reward == 0.0f)
            reward = -1.0f;
        d = this->done;
        if (!d)
            this->newest_reward = 0.0f;
        return std::make_tuple(this->newest_observation, reward, d);
    }

    environment::GameState* LethalGym::Reset() {
        {
            std::lock_guard<std::mutex> lk(this->mutex);
            this->action_queued = false;
        }
        this->condition.notify_one();
        while (!this->game_notified)
            this->condition.notify_one();

        {
            std::unique_lock<std::mutex> lk(this->mutex);

            this->game_notified = false;
            this->condition.wait(lk);
            this->thread_notified = true;
        }

        for (int i = 0; i < 4; i++) {
            if (this->dying_player == i)
                this->state_observer->HealPlayer(i, 9);
            else
                this->state_observer->HealPlayer(i, 8);
        }
        this->dying_player = -1;
        helpers::CodeInjector::SpeedHack(10.0f);
        this->state_observer->SpawnBall();
        // TODO fix this
        this->state_observer->SpawnPlayers();

        {
            std::lock_guard<std::mutex> lk(this->mutex);
            this->action_queued = false;
        }
        this->condition.notify_one();
        while (!this->game_notified)
            this->condition.notify_one();

        {
            std::unique_lock<std::mutex> lk(this->mutex);
            this->game_notified = false;
            this->condition.wait(lk);
            this->thread_notified = true;
            this->done = false;
        }
        return this->newest_observation;
    }

    environment::InputHandler* LethalGym::GetInputHandler() {
        return this->input_handler;
    }

    environment::GameState* LethalGym::GetNewestObservation() {
        return this->newest_observation;
    }

    void LethalGym::SetNewestObservation(environment::GameState *newest_observation) {
        this->newest_observation = newest_observation;
    }

    void LethalGym::CalculateReward(events::Event *event, unsigned int context) {
        int relevant_player = 0;
        if (this->state_observer != NULL && this->state_observer->GetGameState() != NULL) {
            environment::Player** players = this->state_observer->GetGameState()->GetPlayers();
            for (int i = 0; i < 4; i++) {
                if (players[i] != NULL) {
                    unsigned int player_base = players[i]->GetPlayerBase();
                    //std::cout << "Player " << i + 1 << " Base: " << std::hex << player_base << std::endl;
                    if (player_base == context) {
                        this->done = !this->done && players[i]->GetStocks() == 1;
                        relevant_player = i + 1;
                        break;
                    }
                }
            }
        }
        switch (event->GetId()) {
            case 0:
                //Death event
                this->dying_player = relevant_player - 1;
                if (relevant_player == 1) {
                    this->newest_reward -= 300.0f;
                    return;    
                }
                else if (relevant_player == 2) {
                    this->newest_reward += 300.0f;
                    return;
                }
                break;
            case 1:
                //Bunt event
                if (relevant_player == 1) {
                    this->newest_reward += 30.0f;
                    return;
                }
                break;
            case 2:
                //Hit event
                if (relevant_player == 1) {
                    this->newest_reward += 50.0f;
                    return;
                }
        }
    }
}
