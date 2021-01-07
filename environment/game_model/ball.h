#ifndef BALL_H
#define BALL_H

#include <ostream>
#include <vector>

namespace environment {
    class Ball {
        private:
            int x_coord;
            int y_coord;
            int x_velocity;
            int y_velocity;
            int ball_speed;
            int ball_tag;
            int hitstun_countdown;
        public:
            Ball();

            static const int kMaxVelocity = 100000000;
            static const int kMinVelocity = -100000000;

            static const int kMaxBallSpeed = 1000000;

            static const int kMinHitstunCountdown = -2000;
            static const int kMaxHitstunCountdown = 200000;

            int GetXCoord();
            void SetXCoord(int x_coord);
            int GetYCoord();
            void SetYCoord(int y_coord);
            int GetXVelocity();
            void SetXVelocity(int x_velocity);
            int GetYVelocity();
            void SetYVelocity(int y_velocity);
            int GetBallSpeed();
            void SetBallSpeed(int ball_speed);
            int GetBallTag();
            void SetBallTag(int ball_tag);
            int GetHitstunCountdown();
            void SetHitstunCountdown(int hitstun_countdown);

            std::vector<float> NormalizeFloats(int player_count);

            friend std::ostream& operator<<(std::ostream& output, const Ball& H);
    };

}

#endif
