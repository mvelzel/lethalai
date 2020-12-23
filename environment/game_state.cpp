#include "game_state.h"

namespace environment {
    Player* GameState::GetPlayers() {
        return this->players;
    }
    void GameState::SetPlayers(Player players[4]) {
        for (int i = 0; i < 4; i++) {
            this->players[i] = players[i];
        }
    }
}
