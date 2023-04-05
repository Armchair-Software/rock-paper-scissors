#ifndef GAME_STATE_H_INCLUDED
#define GAME_STATE_H_INCLUDED

// forward declarations
namespace render {
class window;
}
namespace game_logic {
class game_logic_base;
}

struct game_state {
  /// Wrap the current game state and key state objects, to pass between emscripten loop functions
  render::window &window;
  game_logic::game_logic_base &logic;
  unsigned int rounds_to_play = 5;
  unsigned int rounds_played = 0;
  unsigned int wins = 0;
  unsigned int draws = 0;
};

#endif // GAME_STATE_H_INCLUDED
