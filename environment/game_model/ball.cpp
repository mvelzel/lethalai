#include "ball.h"

namespace environment {
    Ball::Ball() {
        this->x_coord = 0;
        this->y_coord = 0;
        this->x_velocity = 0;
        this->y_velocity = 0;
        this->ball_speed = 0;
        this->ball_tag = 0;
        this->hitstun_countdown = 0;
    }

    unsigned int Ball::GetXCoord() {
        return this->x_coord;
    }
    void Ball::SetXCoord(unsigned int x_coord) {
        this->x_coord = x_coord;
    }

    unsigned int Ball::GetYCoord() {
        return this->y_coord;
    }
    void Ball::SetYCoord(unsigned int y_coord) {
        this->y_coord = y_coord;
    }

    unsigned int Ball::GetXVelocity() {
        return this->x_velocity;
    }
    void Ball::SetXVelocity(unsigned int x_velocity) {
        this->x_velocity = x_velocity;
    }

    unsigned int Ball::GetYVelocity() {
        return this->y_velocity;
    }
    void Ball::SetYVelocity(unsigned int y_velocity) {
        this->y_velocity = y_velocity;
    }

    unsigned int Ball::GetBallSpeed() {
        return this->ball_speed;
    }
    void Ball::SetBallSpeed(unsigned int ball_speed) {
        this->ball_speed = ball_speed;
    }

    unsigned int Ball::GetBallTag() {
        return this->ball_tag;
    }
    void Ball::SetBallTag(unsigned int ball_tag) {
        this->ball_tag = ball_tag;
    }

    signed int Ball::GetHitstunCountdown() {
        return this->hitstun_countdown;
    }
    void Ball::SetHitstunCountdown(signed int hitstun_countdown) {
        this->hitstun_countdown = hitstun_countdown;
    }

    std::ostream& operator<<(std::ostream& output, const Ball& ball) {
        return output << "Ball{x_coord=" << ball.x_coord <<
            ", y_coord=" << ball.y_coord <<
            ", x_velocity=" << ball.x_velocity <<
            ", y_velocity=" << ball.y_velocity <<
            ", ball_speed=" << ball.ball_speed <<
            ", ball_tag=" << ball.ball_tag <<
            ", hitstun_countdown=" << ball.hitstun_countdown << "}";
    }
}
