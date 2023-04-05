#ifndef RENDER_SCENE_H_INCLUDED
#define RENDER_SCENE_H_INCLUDED

#include <vectorstorm/vector/vector4.h>
#include "game_logic/move.h"

struct game_state;

namespace render {

class scene {
  float hand_travel_opponent{0.0f};                                             // current position of the opponent's hand - smoothly interpolated to the target position below
  float hand_travel_player{  0.0f};                                             // current position of the player's hand

public:
  float hand_target_opponent{0.0f};                                             // target position of the opponent's hand, between 0 for fully retracted to 1 for fully deployed
  float hand_target_player{  0.0f};                                             // target position of the player's hand, also from 0 to 1

  vec4f colour_background{0.5f, 0.5f, 0.5f, 0.0f};                              // current background colour - fade from an initial neutral background
  vec4f colour_background_target{0.2f, 0.2f, 0.2f, 1.0f};                       // which colour we're fading to

  game_logic::move_type opponent_move{game_logic::move_type::scissors};
  game_logic::move_type player_move{  game_logic::move_type::scissors};

  void update();
  void draw(game_state &state) const;
};

}

#endif // RENDER_SCENE_H_INCLUDED
