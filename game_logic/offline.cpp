#include "offline.h"
#include <iostream>
#include <iomanip>
#include <magic_enum.hpp>
#include "strategy/random.h"
#include "strategy/wang_et_al_2014_inverse.h"
#include "strategy/wang_et_al_2014_mod1.h"

namespace game_logic {

void offline::set_difficulty(difficulty_type new_difficulty) {
  /// Update the game difficulty, and initialise a strategy based on it
  game_logic_base::set_difficulty(new_difficulty);                              // base difficulty change effects should apply first

  #pragma GCC diagnostic push
  #pragma GCC diagnostic error "-Wswitch"                                       // enforce exhaustive switch here
  switch(difficulty) {
  #pragma GCC diagnostic pop
  case difficulty_type::easy:
    strategy = std::make_unique<strategy::wang_et_al_2014_inverse>();
    break;
  case difficulty_type::medium:
    strategy = std::make_unique<strategy::random>();
    break;
  case difficulty_type::hard:
    strategy = std::make_unique<strategy::wang_et_al_2014_mod1>();
    break;
  }                                                                             // no default case, to enforce exhaustive switch
}

std::string offline::difficulty_description(difficulty_type target_difficulty) const {
  /// Difficulty descriptions specific to offline local AI play
  #pragma GCC diagnostic push
  #pragma GCC diagnostic error "-Wswitch"                                       // enforce exhaustive switch here
  switch(target_difficulty) {
  #pragma GCC diagnostic pop
  case difficulty_type::easy:
    return "The computer will use strategy to try to lose.";
  case difficulty_type::medium:
    return "The computer will play at random.";
  case difficulty_type::hard:
    return "The computer will use strategy to try to win.";
  }                                                                             // no default case, to enforce exhaustive switch
}

void offline::process() {
  /// Determine and report the opponent's move
  assert(strategy);                                                             // used in lieu of exceptions, as we have no exception support

  opponent_last_move = strategy->get_next_move();                               // ask the current strategy to generate the next move and cache the result
  std::cout << "Offline logic: Opponent move: " << std::quoted(magic_enum::enum_name(opponent_last_move)) << std::endl;
}

void offline::player_move(move_type move) {
  /// Declare a player move
  assert(strategy);                                                             // used in lieu of exceptions, as we have no exception support

  std::cout << "Offline logic: Player move: " << std::quoted(magic_enum::enum_name(move)) << std::endl;
  player_last_move = move;
  strategy->declare_last_player_move(move);                                     // only inform the strategy about the move the player made after it tells us its move, to prevent possibility of cheating strategies

  last_verdict = verdict(player_last_move, opponent_last_move);                 // determine and update the verdict
  std::cout << "Offline logic: Verdict: " << std::quoted(magic_enum::enum_name(last_verdict)) << std::endl;
}

move_type offline::get_last_player_move() const {
  /// Report the player's last move
  return player_last_move;
}

move_type offline::get_last_opponent_move() const {
  /// Report the opponent's last move
  return opponent_last_move;
}

verdict_type offline::get_last_verdict() const {
  /// Report the verdict of the last game
  return last_verdict;
}

void offline::reset() {
  /// Reset our strategy's state, ready for the next game
  if(strategy) strategy->reset();
  // we don't bother resetting our own state, as it's overwritten each move anyway
}

}
