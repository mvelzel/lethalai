#include "player.h"

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
    }

    unsigned int Player::GetXCoord() {
        return this->x_coord;
    }
    void Player::SetXCoord(unsigned int x_coord) {
        this->x_coord = x_coord;
    }

    unsigned int Player::GetYCoord() {
        return this->y_coord;
    }
    void Player::SetYCoord(unsigned int y_coord) {
        this->y_coord = y_coord;
    }

    bool Player::GetFacingDirection() {
        return this->facing_direction;
    }
    void Player::SetFacingDirection(bool facing_direction) {
        this->facing_direction = facing_direction;
    }

    unsigned int Player::GetXVelocity() {
        return this->x_velocity;
    }
    void Player::SetXVelocity(unsigned int x_velocity) {
        this->x_velocity = x_velocity;
    }

    unsigned int Player::GetYVelocity() {
        return this->y_velocity;
    }
    void Player::SetYVelocity(unsigned int y_velocity) {
        this->y_velocity = y_velocity;
    }

    unsigned int Player::GetCharacterState() {
        return this->character_state;
    }
    void Player::SetCharacterState(unsigned int character_state) {
        this->character_state = character_state;
    }

    unsigned int Player::GetAnimationState() {
        return this->animation_state;
    }
    void Player::SetAnimationState(unsigned int animation_state) {
        this->animation_state = animation_state;
    }

    signed int Player::GetHitstunCountdown() {
        return this->hitstun_countdown;
    }
    void Player::SetHitstunCountdown(signed int hitstun_countdown) {
        this->hitstun_countdown = hitstun_countdown;
    }

    unsigned int Player::GetSpecialMeter() {
        return this->special_meter;
    }
    void Player::SetSpecialMeter(unsigned int special_meter) {
        this->special_meter = special_meter;
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
            ", special_meter=" << player.special_meter << "}";
    }
}
