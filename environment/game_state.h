#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_model/player.h"

namespace environment {
    class GameState {
        private:
            Player players[4];
        public:
            Player* GetPlayers();
            void SetPlayers(Player players[4]);

    };
}

#endif
