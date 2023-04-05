#include "game_state.h"
#include <emscripten.h>
#include "game_loops.h"
#include "game_logic/offline.h"
#include "render/scene.h"

void game_state::dispatch_next_loop() {
  /// Direct program flow to the next emscripten quasi-loop, if requested
  switch(next_loop) {
  case next_loop_type::select_difficulty:
    scene.hand_target_opponent = 0.0f;
    scene.hand_target_player = 0.0f;
    rounds_played = 0;
    wins = 0;
    draws = 0;
    logic.reset();
    next_loop = next_loop_type::continue_this;
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(&loop_select_difficulty, this, 0, true);       // loop function, user data, FPS (0 to use browser requestAnimationFrame mechanism), simulate infinite loop
    break;
  case next_loop_type::player_move:
    scene.hand_target_opponent = 0.0f;
    scene.hand_target_player = 0.0f;
    next_loop = next_loop_type::continue_this;
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(&loop_player_move, this, 0, true);
    break;
  case next_loop_type::opponent_move:
    {
      scene.opponent_move = logic.get_last_opponent_move();
      scene.player_move = logic.get_last_player_move();
      scene.hand_target_opponent = 1.0f;
      scene.hand_target_player = 1.0f;
      ++rounds_played;
      auto verdict{logic.get_last_verdict()};
      switch(verdict) {
      case game_logic::verdict_type::win:
        ++wins;
        break;
      case game_logic::verdict_type::draw:
        ++draws;
        break;
      case game_logic::verdict_type::lose:
        // losses are calculated automatically from total, wins and draws
        break;
      }
      next_loop = next_loop_type::continue_this;
      emscripten_cancel_main_loop();
      emscripten_set_main_loop_arg(&loop_opponent_move, this, 0, true);
    }
    break;
  case next_loop_type::continue_this:
    // continue the current loop
  }
}
