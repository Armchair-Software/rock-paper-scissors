#ifndef GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_INVERSE_H_INCLUDED
#define GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_INVERSE_H_INCLUDED

#include "wang_et_al_2014.h"

namespace game_logic::strategy {

class wang_et_al_2014_inverse : public wang_et_al_2014 {
  /// Offline strategy: Structurally based on wang_et_al_2014, but attempts to
  /// play losing moves.  Intentionally losing at RPS is actually just as hard
  /// as intentionally winning, so this strategy may be prone to "failure".

public:
  virtual move_type get_next_move() override final;
};

}

#endif // GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_INVERSE_H_INCLUDED
