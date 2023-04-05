#ifndef GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_H_INCLUDED
#define GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_H_INCLUDED

#include "strategy_base.h"

namespace game_logic::strategy {

class wang_et_al_2014 : public strategy_base {
  /// Offline strategy: as described by Zhijian Wang, Bin Xu, Hai-Jun Zhou:
  /// "Social cycling and conditional responses in the Rock-Paper-Scissors game"
  /// https://arxiv.org/pdf/1404.5199v1.pdf
protected:
  struct past_round {
    move_type our_move;                                                         // this strategy's move this round
    move_type player_move;                                                      // player's move this round
    verdict_type verdict;                                                       // verdict of this round from the player's perspective (a win here is a lose for this strategy)
  };
  std::vector<past_round> past_rounds;                                          // list of previous rounds, most recent last

public:
  virtual move_type get_next_move() override;
  virtual void declare_last_player_move(move_type player_move) override final;
};

}

#endif // GAME_LOGIC_STRATEGY_WANG_ET_AL_2014_H_INCLUDED
