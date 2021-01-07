#include "ball.h"
#include "../game_state.h"
#include "../../helpers/math_helpers.h"

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

    int Ball::GetXCoord() {
        return this->x_coord;
    }
    void Ball::SetXCoord(int x_coord) {
        this->x_coord = x_coord;
    }

    int Ball::GetYCoord() {
        return this->y_coord;
    }
    void Ball::SetYCoord(int y_coord) {
        this->y_coord = y_coord;
    }

    int Ball::GetXVelocity() {
        return this->x_velocity;
    }
    void Ball::SetXVelocity(int x_velocity) {
        this->x_velocity = x_velocity;
    }

    int Ball::GetYVelocity() {
        return this->y_velocity;
    }
    void Ball::SetYVelocity(int y_velocity) {
        this->y_velocity = y_velocity;
    }

    int Ball::GetBallSpeed() {
        return this->ball_speed;
    }
    void Ball::SetBallSpeed(int ball_speed) {
        this->ball_speed = ball_speed;
    }

    int Ball::GetBallTag() {
        return this->ball_tag;
    }
    void Ball::SetBallTag(int ball_tag) {
        this->ball_tag = ball_tag;
    }

    int Ball::GetHitstunCountdown() {
        return this->hitstun_countdown;
    }
    void Ball::SetHitstunCountdown(int hitstun_countdown) {
        this->hitstun_countdown = hitstun_countdown;
    }

    std::vector<float> Ball::NormalizeFloats(int player_count) {
        std::vector<float> result;

        result.push_back(helpers::Math::Normalize(
                    this->x_coord, GameState::kMinX, GameState::kMaxX));
        result.push_back(helpers::Math::Normalize(
                    this->y_coord, GameState::kMinY, GameState::kMaxY));
        result.push_back(helpers::Math::Normalize(
                    this->x_velocity, Ball::kMinVelocity, Ball::kMaxVelocity));
        result.push_back(helpers::Math::Normalize(
                    this->y_velocity, Ball::kMinVelocity, Ball::kMaxVelocity));
        result.push_back(helpers::Math::Normalize(
                    this->ball_speed, 0.0f, Ball::kMaxBallSpeed));
        // TODO fix this for primary/secondary players
        for (int i = 0; i < player_count; i++) {
            if (i == this->ball_tag)
                result.push_back(1.0f);
            else
                result.push_back(0.0f);
        }
        result.push_back(helpers::Math::Normalize(
                    this->hitstun_countdown,
                    Ball::kMinHitstunCountdown, Ball::kMaxHitstunCountdown));
        

        return result;
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
