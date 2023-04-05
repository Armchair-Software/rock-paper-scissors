#include <boost/throw_exception.hpp>
#include <iostream>
#include "render/window.h"
#include "render/texture_manager.h"
#include "gui/gui.h"
#include "game_state.h"
#include "game_logic/offline.h"
#include "get_version.h"

#ifdef BOOST_NO_EXCEPTIONS
void boost::throw_exception(std::exception const & e) {
  /// Custom exception replacement function when boost exceptions are disabled
  std::cout << "ERROR: Boost would have thrown exception: " << e.what() << std::endl;
  abort();
}
#endif // BOOST_NO_EXCEPTIONS

[[noreturn]] auto main()->int {                                                 // noreturn here is not standards-compliant, but is appropriate for emscripten with a main loop
  render::window window;
  window.set_window_title("Loading: RPS version " + get_version());
  if(!window.init()) {
    std::cerr << "ERROR: Failed to create a window, cannot continue." << std::endl;
    EM_ASM(
      if(confirm("Error - unable to create a graphics window.  Please check that WebGL 2.0 is supported by your browser, and is enabled.  Press OK to visit a validation page.")) {
        window.location.replace("https://get.webgl.org/webgl2/");
      }
    );
    std::abort();
  }
  window.set_window_title("RPS version " + get_version() + " by Eugene Hopkinson");

  render::texture_manager textures;                                             // load the textures
  gui::init(window);                                                            // set up the GUI
  game_logic::offline logic;                                                    // game logic: offline local AI

  game_state state{                                                             // package references to game state for the loops
    window,
    textures,
    logic
  };
  state.next_loop = game_state::next_loop_type::select_difficulty;
  state.dispatch_next_loop();
  std::unreachable();                                                           // execution never returns to this point
}
