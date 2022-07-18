#ifndef PLAYER_H
#define PLAYER_H

#include <ostream>
#include <Windows.h>
#include <array>

namespace environment {
    class Player {
        private:
            int x_coord;
            int y_coord;
            bool facing_direction;
            int x_velocity;
            int y_velocity;
            int character_state;
            int animation_state;
            int hitstun_countdown;
            int special_meter;
            int stocks;
            DWORD player_base;
        public:
            Player();

            static const int kAnimationStateCount = 42;
            static const int kCharacterStateCount = 20;

            static const int kStateSize = 8 + kAnimationStateCount + kCharacterStateCount;

            static const int kMaxVelocity = 10000000;
            static const int kMinVelocity = -10000000;
            
            static const int kMaxSpecialMeter = 6553600;
            static const int kMaxStocks = 8;

            int GetXCoord();
            void SetXCoord(signed int x_coord);
            int GetYCoord();
            void SetYCoord(signed int y_coord);
            bool GetFacingDirection();
            void SetFacingDirection(bool facing_direction);
            int GetXVelocity();
            void SetXVelocity(int x_velocity);
            int GetYVelocity();
            void SetYVelocity (int y_velocity);
            int GetCharacterState();
            void SetCharacterState(int character_state);
            int GetAnimationState();
            void SetAnimationState(int animation_state);
            int GetHitstunCountdown();
            void SetHitstunCountdown(int hitstun_countdown);
            int GetSpecialMeter();
            void SetSpecialMeter(int special_meter);
            DWORD GetPlayerBase();
            void SetPlayerBase(DWORD player_base);
            int GetStocks();
            void SetStocks(int stocks);

            std::array<float, Player::kStateSize> NormalizeFloats();

            friend std::ostream& operator<<(std::ostream& output, const Player& player);
    };
}

#endif
