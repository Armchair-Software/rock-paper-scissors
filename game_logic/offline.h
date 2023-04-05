#ifndef GAME_LOGIC_OFFLINE_H_INCLUDED
#define GAME_LOGIC_OFFLINE_H_INCLUDED

#include "game_logic_base.h"
#include "strategy/strategy_base.h"

namespace game_logic {

class offline : public game_logic_base {
  /// Game logic: local AI for offline play
  std::unique_ptr<strategy::strategy_base> strategy;                            // the strategy we will use - dynamically chosen based on requested difficulty

  move_type player_last_move;                                                   // the player's last move
  move_type opponent_last_move;                                                 // cache of the opponent's last move
  verdict_type last_verdict;                                                    // cache of the last game outcome (win, draw, lose)

public:
  virtual void set_difficulty(difficulty_type new_difficulty) override final;

  virtual std::string difficulty_description(difficulty_type difficulty) const override final;

  virtual void process() override final;
  virtual void player_move(move_type move) override final;
  virtual move_type get_last_player_move() const override final;
  virtual move_type get_last_opponent_move() const override final;
  virtual verdict_type get_last_verdict() const override final;

  virtual void reset() override final;
};

}

#endif // GAME_LOGIC_OFFLINE_H_INCLUDED
