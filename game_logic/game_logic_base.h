#ifndef GAME_LOGIC_GAME_LOGIC_BASE_H_INCLUDED
#define GAME_LOGIC_GAME_LOGIC_BASE_H_INCLUDED

#include <string>
#include "difficulty.h"
#include "move.h"

namespace game_logic {

class game_logic_base {
  /// Base class for game logic flows
public:
  difficulty_type difficulty{difficulty_type::medium};

protected:
  game_logic_base() = default;
public:
  virtual ~game_logic_base() = default;
  virtual void set_difficulty(difficulty_type new_difficulty);

  virtual std::string difficulty_description(difficulty_type difficulty) const;

  virtual void process() = 0;
  virtual void player_move(move_type move) = 0;
  virtual move_type get_last_player_move() const = 0;
  virtual move_type get_last_opponent_move() const = 0;
  virtual verdict_type get_last_verdict() const = 0;

  virtual void reset() = 0;
};

}

#endif // GAME_LOGIC_GAME_LOGIC_BASE_H_INCLUDED
