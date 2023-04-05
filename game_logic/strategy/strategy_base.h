#ifndef GAME_LOGIC_STRATEGY_STRATEGY_BASE_H_INCLUDED
#define GAME_LOGIC_STRATEGY_STRATEGY_BASE_H_INCLUDED

#include "game_logic/move.h"

namespace game_logic::strategy {

class strategy_base {
  /// Pure virtual base for local (offline) AI opponent strategies
protected:
  strategy_base() = default;
public:
  virtual ~strategy_base() = default;

  virtual move_type get_next_move() = 0;                                        // generate the next move - for fairness, must happen before being told the player's simultaneous move
  virtual void declare_last_player_move(move_type player_move) = 0;             // get notified about what move the player just made - strategies must work out success/failure state themselves, if they care about it

  virtual void reset() = 0;
};

}

#endif // GAME_LOGIC_STRATEGY_STRATEGY_BASE_H_INCLUDED
