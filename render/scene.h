#ifndef RENDER_SCENE_H_INCLUDED
#define RENDER_SCENE_H_INCLUDED

#include "game_logic/move.h"

struct game_state;

namespace render {

class scene {
  float hand_travel_opponent{0.0f};
  float hand_travel_player{  0.0f};

public:
  float hand_target_opponent{0.0f};
  float hand_target_player{  0.0f};

  game_logic::move_type opponent_move{game_logic::move_type::scissors};
  game_logic::move_type player_move{  game_logic::move_type::scissors};

  void update();
  void draw(game_state &state) const;
};

}

#endif // RENDER_SCENE_H_INCLUDED
