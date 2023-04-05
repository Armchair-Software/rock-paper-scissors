#ifndef GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_MOD1_H_INCLUDED
#define GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_MOD1_H_INCLUDED

#include "wang_et_al_2014.h"

namespace game_logic::strategy {

class wang_et_al_2014_mod1 : public wang_et_al_2014 {
  /// Offline strategy: Based on Wang et al 2014, with the following modifications:
  /// If the strategy loses x times in a row, play a random move next

  static constexpr unsigned int max_losing_moves_before_change{2};

public:
  virtual move_type get_next_move() override final;
};

}

#endif // GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_MOD1_H_INCLUDED
