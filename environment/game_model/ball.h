#ifndef BALL_H
#define BALL_H

#include <ostream>

namespace environment {
    class Ball {
        private:
            unsigned int x_coord;
            unsigned int y_coord;
            unsigned int x_velocity;
            unsigned int y_velocity;
            unsigned int ball_speed;
            unsigned int ball_tag;
            signed int hitstun_countdown;
        public:
            Ball();

            unsigned int GetXCoord();
            void SetXCoord(unsigned int x_coord);
            unsigned int GetYCoord();
            void SetYCoord(unsigned int y_coord);
            unsigned int GetXVelocity();
            void SetXVelocity(unsigned int x_velocity);
            unsigned int GetYVelocity();
            void SetYVelocity(unsigned int y_velocity);
            unsigned int GetBallSpeed();
            void SetBallSpeed(unsigned int ball_speed);
            unsigned int GetBallTag();
            void SetBallTag(unsigned int ball_tag);
            signed int GetHitstunCountdown();
            void SetHitstunCountdown(signed int hitstun_countdown);

            friend std::ostream& operator<<(std::ostream& output, const Ball& H);
    };

}

#endif
