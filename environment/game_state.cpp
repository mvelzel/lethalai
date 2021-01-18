#include "game_state.h"

#include <iostream>

namespace environment {
    GameState::GameState() {
        this->ball = new Ball();
        for (int i = 0; i < 4; i++) {
            this->players[i] = new Player();
        }
    }

    Player** GameState::GetPlayers() {
        return this->players;
    }
    void GameState::SetPlayers(Player* players[4]) {
        for (int i = 0; i < 4; i++) {
            this->players[i] = players[i];
        }
    }

    Ball* GameState::GetBall() {
        return this->ball;
    }
    void GameState::SetBall(Ball* ball) {
        this->ball = ball;
    }

    std::vector<float> GameState::NormalizeFloats(int first_player) {
        if (this->players[first_player] == NULL) {
            std::cout << "ERROR: Undefined first_player" << std::endl;
            return std::vector<float>();
        }
        std::vector<float> player_first = this->players[first_player]->NormalizeFloats();
        //std::cout << *this->players[first_player] << std::endl;
        //std::copy(player_first.begin(), player_first.end(), std::ostream_iterator<float>(std::cout, " "));
        //std::cout << std::endl;
        int count = 1;
        for (int i = 0; i < 4; i++) {
            if (i != first_player && this->players[i] != NULL) {
                count++;
                std::vector<float> p_vector = this->players[i]->NormalizeFloats();
                //std::cout << *this->players[i] << std::endl;
                //std::copy(p_vector.begin(), p_vector.end(), std::ostream_iterator<float>(std::cout, " "));
                //std::cout << std::endl;
                player_first.insert(player_first.end(), p_vector.begin(), p_vector.end());
            }
        }
        std::vector<float> b_vector = this->ball->NormalizeFloats(count);
        //std::cout << *this->ball << std::endl;
        //std::copy(b_vector.begin(), b_vector.end(), std::ostream_iterator<float>(std::cout, " "));
        //std::cout << std::endl;
        //std::cout << std::endl;
        player_first.insert(player_first.end(), b_vector.begin(), b_vector.end());

        return player_first;
    }
}
