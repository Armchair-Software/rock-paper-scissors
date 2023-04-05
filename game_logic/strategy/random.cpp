#include "random.h"
#include <iostream>

namespace game_logic::strategy {

move_type random::get_next_move() {
  /// Generate the next move entirely at random
  auto const move_type_index{move_type_dist(random_engine)};                    // generate a random index for move types
  std::cout << "Random strategy: Rolled a " << move_type_index << std::endl;
  return static_cast<move_type>(move_type_index);
}

void random::declare_last_player_move(move_type) {
  /// Get told about the player's last move - random strategy doesn't care, so do nothing with it
  // noop
}

}
