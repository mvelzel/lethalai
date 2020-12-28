#ifndef PLAYER_H
#define PLAYER_H

#include<ostream>

namespace environment {
    class Player {
        private:
            unsigned int x_coord;
            unsigned int y_coord;
            bool facing_direction;
            unsigned int x_velocity;
            unsigned int y_velocity;
            unsigned int character_state;
            unsigned int animation_state;
            signed int hitstun_countdown;
            unsigned int special_meter;
        public:
            Player();

            unsigned int GetXCoord();
            void SetXCoord(unsigned int x_coord);
            unsigned int GetYCoord();
            void SetYCoord(unsigned int y_coord);
            bool GetFacingDirection();
            void SetFacingDirection(bool facing_direction);
            unsigned int GetXVelocity();
            void SetXVelocity(unsigned int x_velocity);
            unsigned int GetYVelocity();
            void SetYVelocity (unsigned int y_velocity);
            unsigned int GetCharacterState();
            void SetCharacterState(unsigned int character_state);
            unsigned int GetAnimationState();
            void SetAnimationState(unsigned int animation_state);
            signed int GetHitstunCountdown();
            void SetHitstunCountdown(signed int hitstun_countdown);
            unsigned int GetSpecialMeter();
            void SetSpecialMeter(unsigned int special_meter);

            friend std::ostream& operator<<(std::ostream& output, const Player& player);
    };
}

#endif
