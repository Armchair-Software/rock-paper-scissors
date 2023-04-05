#include "wang_et_al_2014_inverse.h"
#include <iostream>
#include <iomanip>
#include "random.h"

namespace game_logic::strategy {

move_type wang_et_al_2014_inverse::get_next_move() {
  /// Generate the next move to lose, according to the strategy
  move_type move;

  if(past_rounds.empty()) {
    // strategy: this is the first round - play a random move
    random random_strategy;                                                     // instantiate a random strategy for this move by itself
    move = random_strategy.get_next_move();                                     // outsource our randomness to the random strategy
    std::cout << "Wang et al 2014 strategy: random first move: " << std::quoted(magic_enum::enum_name(move)) << std::endl;
  } else {
    switch(past_rounds.back().verdict) {
    case verdict_type::win:                                                     // player won, strategy lost
      // base strategy: if you lose, switch to the thing that beats the thing your opponent just played
      // inverse strategy: if you lose, keep playing the same move
      move = past_rounds.back().our_move;
      std::cout << "Wang et al 2014 inverse strategy: lost last round to " << magic_enum::enum_name(past_rounds.back().player_move) << ", so playing " << std::quoted(magic_enum::enum_name(move)) << " again" << std::endl;
      break;
    case verdict_type::draw:
    case verdict_type::lose:                                                    // player lost, strategy won
      // base strategy: if you win, switch to the thing that would beat the thing that you just played
      // inverse strategy: if you win, switch to the thing that beat you
      move = past_rounds.back().player_move;
      std::cout << "Wang et al 2014 inverse strategy: won last round against " << magic_enum::enum_name(past_rounds.back().player_move) << ", so playing " << std::quoted(magic_enum::enum_name(move)) << " in turn" << std::endl;
      break;
    }
  }

  past_rounds.emplace_back(past_round{move, {}, {}});
  return move;
}

}
