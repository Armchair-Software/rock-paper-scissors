#ifndef GAME_LOGIC_STRATEGY_RANDOM_H_INCLUDED
#define GAME_LOGIC_STRATEGY_RANDOM_H_INCLUDED

#include <random>
#include "strategy_base.h"

namespace game_logic::strategy {

class random : public strategy_base {
  /// Offline strategy: play entirely at random without considering past moves
  std::random_device random_device;
  std::default_random_engine random_engine{random_device()};
  std::uniform_int_distribution<unsigned int> move_type_dist{0, magic_enum::enum_count<move_type>() - 1};


public:
  virtual move_type get_next_move() override final;
  virtual void declare_last_player_move(move_type player_move) override final;

  virtual void reset() override final;
};

}

#endif // GAME_LOGIC_STRATEGY_RANDOM_H_INCLUDED
