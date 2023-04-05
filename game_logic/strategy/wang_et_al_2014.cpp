#include "wang_et_al_2014.h"
#include <iostream>
#include <iomanip>
#include "random.h"

namespace game_logic::strategy {

static move_type beats_move(move_type move) {
  /// Return the move that would beats the given move
  auto const beats_move_type_index{(std::to_underlying(move) + 1) % magic_enum::enum_count<move_type>()};
  return magic_enum::enum_cast<move_type>(beats_move_type_index).value();       // cyclical - the next item always beats the previous
}

move_type wang_et_al_2014::get_next_move() {
  /// Generate the next move according to the strategy: https://arxiv.org/pdf/1404.5199v1.pdf
  move_type move;

  if(past_rounds.empty()) {
    // strategy: this is the first round - play a random move
    random random_strategy;                                                     // instantiate a random strategy for this move by itself
    move = random_strategy.get_next_move();                                     // outsource our randomness to the random strategy
    std::cout << "Wang et al 2014 strategy: random first move: " << std::quoted(magic_enum::enum_name(move)) << std::endl;
  } else {
    switch(past_rounds.back().verdict) {
    case verdict_type::win:                                                     // player won, strategy lost
      // strategy: if you lose, switch to the thing that beats the thing your opponent just played
      move = beats_move(past_rounds.back().player_move);
      std::cout << "Wang et al 2014 strategy: lost last round to " << magic_enum::enum_name(past_rounds.back().player_move) << ", so playing " << std::quoted(magic_enum::enum_name(move)) << " which beats it" << std::endl;
      break;
    case verdict_type::draw:
    case verdict_type::lose:                                                    // player lost, strategy won
      // strategy: if you win, switch to the thing that would beat the thing that you just played
      move = beats_move(past_rounds.back().our_move);
      std::cout << "Wang et al 2014 strategy: won last round with " << magic_enum::enum_name(past_rounds.back().our_move) << ", so playing " << std::quoted(magic_enum::enum_name(move)) << " which beats it" << std::endl;
      break;
    }
  }

  past_rounds.emplace_back(past_round{move, {}, {}});
  return move;
}

void wang_et_al_2014::declare_last_player_move(move_type move) {
  /// Get told about the player's last move, and update our verdict history
  past_rounds.back().player_move = move;
  past_rounds.back().verdict = verdict(past_rounds.back().player_move, past_rounds.back().our_move);
}

}
