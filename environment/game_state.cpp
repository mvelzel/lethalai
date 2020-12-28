#include "game_state.h"

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
}
