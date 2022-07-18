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

  std::array<float, Ball::kStateSize> Ball::NormalizeFloats() {
    std::array<float, Ball::kStateSize> result{};

    result[0] = helpers::Math::Normalize(
        this->x_coord, GameState::kMinX, GameState::kMaxX);
    result[1] = helpers::Math::Normalize(
        this->y_coord, GameState::kMinY, GameState::kMaxY);
    result[2] = helpers::Math::NormalizeRange(
        this->x_velocity, Ball::kMinVelocity, Ball::kMaxVelocity, -1.0f, 1.0f);
    result[3] = helpers::Math::NormalizeRange(
        this->y_velocity, Ball::kMinVelocity, Ball::kMaxVelocity, -1.0f, 1.0f);
    result[4] = helpers::Math::Normalize(
        this->ball_speed, 0.0f, Ball::kMaxBallSpeed);
    result[5] = helpers::Math::Normalize(
        this->hitstun_countdown,
        Ball::kMinHitstunCountdown, Ball::kMaxHitstunCountdown);
    result[6 + this->ball_tag] = 1.0f;
    // TODO fix this for primary/secondary players

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
