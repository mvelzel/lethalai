#include "lethal_gym.h"
#include <Windows.h>
#include <iostream>
#include <math.h>
#include <mutex>
#include <tuple>
#include "../helpers/code_injector.h"
#include "../helpers/math_helpers.h"

namespace gym {
    LethalGym* global_lethal_gym;

    void HandleEvent(events::Event* event, unsigned int context) {
        //std::cout << "Event triggered, eventnum: " << event->GetId() << ", " <<
        //    "context: " << std::hex << context << std::endl;
        global_lethal_gym->CalculateReward(event, context);
    }

    void HandleObservation(environment::GameState* game_state) {
        //std::cout << "Game Called" << std::endl;
        if (game_state->GetBall() == NULL)
            return;
        environment::Player** players = game_state->GetPlayers();
        bool cont = false;
        for (int i = 0; i < 4; i++) {
            environment::Player* player = players[i];
            if (player != NULL) {
                cont = true;
                if (!global_lethal_gym->done && (player->GetCharacterState() == 18 ||
                            game_state->GetBall()->GetBallSpeed() <= 0 ||
                            game_state->GetBall()->GetXCoord() > environment::GameState::kMaxX ||
                            game_state->GetBall()->GetXCoord() < environment::GameState::kMinX ||
                            game_state->GetBall()->GetYCoord() > environment::GameState::kMaxY ||
                            game_state->GetBall()->GetYCoord() < environment::GameState::kMinY ||
                            player->GetYCoord() > environment::GameState::kMaxY ||
                            player->GetYCoord() < environment::GameState::kMinY ||
                            player->GetXCoord() > environment::GameState::kMaxX ||
                            player->GetXCoord() < environment::GameState::kMinX))
                    return;
            }
        }
        if (!cont)
            return;
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
        this->graphics_handler = new graphics::GraphicsHandler();
        this->info_list = new graphics::InfoList(0, 0);
        this->graphics_handler->AddDrawable(info_list);
        this->state_observer = new environment::StateObserver(true);
        this->event_observer = new environment::EventObserver(true);
        this->event_observer->SetObserver(HandleEvent);
        this->state_observer->SetObserver(HandleObservation);
    }

    std::tuple<environment::GameState*, float*, bool> LethalGym::Step(
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

        float reward[2] = { this->newest_reward[0], this->newest_reward[1] };
        this->newest_reward[0] = 0.0f;
        this->newest_reward[1] = 0.0f;
        //TODO separate state-based rewards
        bool is_done[2] = { false, false };
        for (int i = 0; i < 2; i++) {
            if (reward[i] == 0.0f) {
                //reward = -1.0f;
                this->steps_since_reward[i] += 1;
                float x1 = this->newest_observation->GetPlayers()[i]->GetXCoord();
                float y1 = this->newest_observation->GetPlayers()[i]->GetYCoord();
                float x2 = this->newest_observation->GetBall()->GetXCoord();
                float y2 = this->newest_observation->GetBall()->GetYCoord();
                float dist = sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
                float m = 78118913.0f;
                float norm = helpers::Math::NormalizeRange(dist, 0, m,
                        0.0f, 0.5f);
                reward[i] = -norm + 0.2f;
            } else {
                this->steps_since_reward[i] = 0;
            }
            if (this->steps_since_reward[i] >= this->kMaxEmptySteps) {
                is_done[i] = true;
                //reward = -1000.0f;
            }
        }
        if (is_done[0] && is_done[1])
            this->done = true;
        d = this->done;
        return std::make_tuple(this->newest_observation, reward, d);
    }

    environment::GameState* LethalGym::Reset() {
        {
            std::lock_guard<std::mutex> lk(this->mutex);
            this->action_queued = false;
            this->steps_since_reward[0] = 0;
            this->steps_since_reward[1] = 0;
            this->newest_reward[0] = 0;
            this->newest_reward[1] = 0;
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

        this->dying_player = -1;
        helpers::CodeInjector::SpeedHack(10.0f);
        this->state_observer->SpawnBall();
        this->state_observer->HealPlayers(8);
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
                        this->done = !this->done && event->GetId() == 0 && 
                            players[i]->GetStocks() == 1;
                        if (done) {
                            for (int j = 0; j < 4; j++) {
                                if (j == i)
                                    this->state_observer->HealPlayer(j, 8);
                                else
                                    this->state_observer->HealPlayer(j, 8);
                            }
                        }
                        relevant_player = i;
                        break;
                    }
                }
            }
        }
        switch (event->GetId()) {
            case 0:
                //Death event
                this->dying_player = relevant_player;
                if (relevant_player == 0) {
                    this->newest_reward[0] -= 400.0f;
                    this->newest_reward[1] += 400.0f;
                    this->info_list->SetRecentP1(-400.0f);
                    this->info_list->SetRecentP2(400.0f);
                    return;
                }
                else if (relevant_player == 1) {
                    this->newest_reward[1] -= 400.0f;
                    this->newest_reward[0] += 400.0f;
                    this->info_list->SetRecentP1(400.0f);
                    this->info_list->SetRecentP2(-400.0f);
                    return;
                }
                break;
            case 1:
                //Bunt event
                if (relevant_player == 0) {
                    this->newest_reward[0] += 50.0f;
                    this->info_list->SetRecentP1(50.0f);
                    return;
                } else if (relevant_player == 1) {
                    this->newest_reward[1] += 50.0f;
                    this->info_list->SetRecentP2(50.0f);
                    return;
                }
                break;
            case 2:
                //Hit event
                if (relevant_player == 0) {
                    this->newest_reward[0] += 80.0f;
                    this->info_list->SetRecentP1(80.0f);
                    return;
                } else if (relevant_player == 1) {
                    this->newest_reward[1] += 80.0f;
                    this->info_list->SetRecentP2(80.0f);
                    return;
                }
                break;
        }
    }
}
