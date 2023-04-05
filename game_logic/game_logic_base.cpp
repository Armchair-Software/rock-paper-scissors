#include "game_logic_base.h"
#include <iostream>
#include <iomanip>
#include <magic_enum.hpp>

namespace game_logic {

void game_logic_base::set_difficulty(difficulty_type new_difficulty) {
  /// Update the game difficulty
  std::cout << "Difficulty selected: " << std::quoted(magic_enum::enum_name(new_difficulty)) << std::endl;
  difficulty = new_difficulty;
}

std::string game_logic_base::difficulty_description(difficulty_type target_difficulty) const {
  /// Generic difficulty descriptions - overridden by more specific descriptions
  /// in different game logic engines
  switch(target_difficulty) {
  case difficulty_type::easy:
    return "Easy gameplay.";
  case difficulty_type::medium:
    return "Moderate difficulty.";
  case difficulty_type::hard:
    return "Difficult gameplay.";
  }
}

}
