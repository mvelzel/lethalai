#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_model/player.h"
#include "game_model/ball.h"
#include <array>

namespace environment {
    class GameState {
        private:
            Player *players[4] = { NULL, NULL, NULL, NULL };
            Ball *ball = NULL;
        public:
            GameState();

            static const int kMinX = 2162688;
            static const int kMaxX = 81723392;

            static const int kMinY = 2162688;
            static const int kMaxY = 33423360;

            static const int kStateSize = Player::kStateSize * 4 + Ball::kStateSize;

            Player** GetPlayers();
            void SetPlayers(Player* players[4]);
            Ball* GetBall();
            void SetBall(Ball* ball);
            std::array<float, GameState::kStateSize> NormalizeFloats(int first_player);
    };
}

#endif
