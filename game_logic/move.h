#ifndef GAME_LOGIC_MOVE_H_INCLUDED
#define GAME_LOGIC_MOVE_H_INCLUDED

#include <string>
#include <magic_enum.hpp>
#include "render/texture_manager.h"

namespace game_logic {

enum class move_type : unsigned int {
  /// Permitted game moves
  rock,
  paper,
  scissors,
};

enum class verdict_type : unsigned int {
  /// Permitted game outcomes
  win,
  draw,
  lose,
};


std::string describe_move(move_type move);
render::texture_manager::texture_id get_icon_texture_id(move_type move);

constexpr verdict_type verdict(move_type player_move, move_type opponent_move) {
  /// Return the verdict of a player and opponent move combination
  if(player_move == opponent_move) return verdict_type::draw;                   // same moves mean a draw

  auto const next_move_type_index{(std::to_underlying(player_move) + 1) % magic_enum::enum_count<move_type>()};
  return (opponent_move == magic_enum::enum_cast<move_type>(next_move_type_index).value()) // cyclical - the next item always beats the previous
         ? verdict_type::lose
         : verdict_type::win;

  return verdict_type::draw;
}

}

#endif // GAME_LOGIC_MOVE_H_INCLUDED
