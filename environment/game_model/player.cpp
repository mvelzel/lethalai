#include "player.h"
#include "../../helpers/math_helpers.h"
#include "../game_state.h"

namespace environment {
  Player::Player() {
    this->x_coord = 0;
    this->y_coord = 0;
    this->facing_direction = 0;
    this->x_velocity = 0;
    this->y_velocity = 0;
    this->character_state = 0;
    this->animation_state = 0;
    this->hitstun_countdown = 0;
    this->special_meter = 0;
    this->player_base = 0;
    this->stocks = 0;

  }

  int Player::GetXCoord() {
    return this->x_coord;
  }
  void Player::SetXCoord(int x_coord) {
    this->x_coord = x_coord;
  }

  int Player::GetYCoord() {
    return this->y_coord;
  }
  void Player::SetYCoord(int y_coord) {
    this->y_coord = y_coord;
  }

  bool Player::GetFacingDirection() {
    return this->facing_direction;
  }
  void Player::SetFacingDirection(bool facing_direction) {
    this->facing_direction = facing_direction;
  }

  int Player::GetXVelocity() {
    return this->x_velocity;
  }
  void Player::SetXVelocity(int x_velocity) {
    this->x_velocity = x_velocity;
  }

  int Player::GetYVelocity() {
    return this->y_velocity;
  }
  void Player::SetYVelocity(int y_velocity) {
    this->y_velocity = y_velocity;
  }

  int Player::GetCharacterState() {
    return this->character_state;
  }
  void Player::SetCharacterState(int character_state) {
    this->character_state = character_state;
  }

  int Player::GetAnimationState() {
    return this->animation_state;
  }
  void Player::SetAnimationState(int animation_state) {
    this->animation_state = animation_state;
  }

  int Player::GetHitstunCountdown() {
    return this->hitstun_countdown;
  }
  void Player::SetHitstunCountdown(int hitstun_countdown) {
    this->hitstun_countdown = hitstun_countdown;
  }

  int Player::GetSpecialMeter() {
    return this->special_meter;
  }
  void Player::SetSpecialMeter(int special_meter) {
    this->special_meter = special_meter;
  }

  DWORD Player::GetPlayerBase() {
    return this->player_base;
  }
  void Player::SetPlayerBase(DWORD player_base) {
    this->player_base = player_base;
  }

  int Player::GetStocks() {
    return this->stocks;
  }
  void Player::SetStocks(int stocks) {
    this->stocks = stocks;
  }

  std::array<float, Player::kStateSize> Player::NormalizeFloats() {
    std::array<float, Player::kStateSize> result{};

    result[0] = helpers::Math::Normalize(
        this->x_coord, GameState::kMinX, GameState::kMaxX);
    result[1] = helpers::Math::Normalize(
        this->y_coord, GameState::kMinY, GameState::kMaxY);
    result[2] = this->facing_direction;
    result[3] = helpers::Math::NormalizeRange(
        this->x_velocity, Player::kMinVelocity, Player::kMaxVelocity, -1.0f, 1.0f);
    result[4] = helpers::Math::NormalizeRange(
          this->y_velocity, Player::kMinVelocity, Player::kMaxVelocity, -1.0f, 1.0f);
    result[5] = helpers::Math::Normalize(
        this->hitstun_countdown,
        Ball::kMinHitstunCountdown, Ball::kMaxHitstunCountdown);
    result[6] = helpers::Math::Normalize(
        this->special_meter, 0.0f, Player::kMaxSpecialMeter);
    result[7] = helpers::Math::Normalize(
        this->stocks, 0.0f, Player::kMaxStocks);
    result[8 + this->character_state] = 1.0f;
    result[8 + this->character_state + this->animation_state] = 1.0f;

    return result;
  }

  std::ostream& operator<<(std::ostream& output, const Player& player) {
    return output << "Player{x_coord=" << player.x_coord <<
      ", y_coord=" << player.y_coord <<
      ", facing_direction=" << (int) player.facing_direction <<
      ", x_velocity=" << player.x_velocity <<
      ", y_velocity=" << player.y_velocity <<
      ", character_state=" << player.character_state <<
      ", animation_state=" << player.animation_state <<
      ", hitstun_countdown=" << player.hitstun_countdown <<
      ", special_meter=" << player.special_meter <<
      ", player_base=" << player.player_base << "}";
  }
}
