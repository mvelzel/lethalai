#include "player.h"

namespace environment {
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
}
