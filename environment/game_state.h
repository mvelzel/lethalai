#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_model/player.h"
#include "game_model/ball.h"

namespace environment {
    class GameState {
        private:
            Player *players[4];
            Ball *ball;
        public:
            GameState();

            Player** GetPlayers();
            void SetPlayers(Player* players[4]);
            Ball* GetBall();
            void SetBall(Ball* ball);
    };
}

#endif
