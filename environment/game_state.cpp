#include "game_state.h"

#include <iostream>
#include <algorithm>

namespace environment {
  GameState::GameState() {
    this->ball = new Ball();
    for (int i = 0; i < 4; i++) {
      this->players[i] = new Player();
    }
  }

  Player** GameState::GetPlayers() {
    return this->players;
  }
  void GameState::SetPlayers(Player* players[4]) {
    for (int i = 0; i < 4; i++) {
      this->players[i] = players[i];
    }
  }

  Ball* GameState::GetBall() {
    return this->ball;
  }
  void GameState::SetBall(Ball* ball) {
    this->ball = ball;
  }

  std::array<float, GameState::kStateSize> GameState::NormalizeFloats(int first_player) {
    std::array<float, GameState::kStateSize> result{};
    if (this->players[first_player] == NULL) {
      std::cout << "ERROR: Undefined first_player" << std::endl;
      return result;
    }
    std::copy_n(this->players[first_player]->NormalizeFloats().begin(), Player::kStateSize, result.begin());
      //std::cout << *this->players[first_player] << std::endl;
      //std::copy(player_first.begin(), player_first.end(), std::ostream_iterator<float>(std::cout, " "));
      //std::cout << std::endl;
    int count = 1;
    for (int i = 0; i < 4; i++) {
      if (i != first_player && this->players[i] != NULL) {
        //std::cout << *this->players[i] << std::endl;
        //std::copy(p_vector.begin(), p_vector.end(), std::ostream_iterator<float>(std::cout, " "));
        //std::cout << std::endl;
        std::copy_n(
            this->players[i]->NormalizeFloats().begin(),
            Player::kStateSize,
            result.begin() + Player::kStateSize * count);
      }
      if (i != first_player)
        count++;
    }
    std::copy_n(this->ball->NormalizeFloats().begin(),
        Ball::kStateSize,
        result.begin() + Player::kStateSize * 4);
    //std::cout << *this->ball << std::endl;
    //std::copy(b_vector.begin(), b_vector.end(), std::ostream_iterator<float>(std::cout, " "));
    //std::cout << std::endl;
    //std::cout << std::endl;

    return result;
  }
}
