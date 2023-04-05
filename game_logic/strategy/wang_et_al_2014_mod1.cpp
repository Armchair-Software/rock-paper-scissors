#include "wang_et_al_2014_mod1.h"
#include <iostream>
#include <iomanip>
#include "random.h"

namespace game_logic::strategy {

move_type wang_et_al_2014_mod1::get_next_move() {
  /// Generate the next move according to the strategy: https://arxiv.org/pdf/1404.5199v1.pdf
  if(past_rounds.size() < max_losing_moves_before_change) {                     // we haven't played enough games yet
    return wang_et_al_2014::get_next_move();
  }

  auto it{past_rounds.rbegin()};
  bool last_x_rounds_won_by_player = true;
  for(unsigned int i = 0; i != max_losing_moves_before_change; ++i) {
    if(it->verdict != verdict_type::win) {
      last_x_rounds_won_by_player = false;
      break;
    }
    ++it;
  }

  if(last_x_rounds_won_by_player) {
    std::cout << "Wang et al 2014 modified strategy: lost last " << max_losing_moves_before_change << " rounds with Wang et al 2014 strategy, making a random move this time instead" << std::endl;
    random random_strategy;                                                     // instantiate a random strategy for this move by itself
    auto const move{random_strategy.get_next_move()};                           // outsource our randomness to the random strategy
    past_rounds.emplace_back(past_round{move, {}, {}});                         // cache the move
    return move;
  } else {
    return wang_et_al_2014::get_next_move();
  }
}

}
