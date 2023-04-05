#include <boost/throw_exception.hpp>
#include <iostream>
#include <emscripten.h>
#include "render/window.h"
#include "get_version.h"


#ifdef BOOST_NO_EXCEPTIONS
void boost::throw_exception(std::exception const & e) {
  /// Custom exception replacement function when boost exceptions are disabled
  std::cout << "ERROR: Boost would have thrown exception: " << e.what() << std::endl;
  abort();
}
#endif // BOOST_NO_EXCEPTIONS

static void loop_select_difficulty(void *data);
static void loop_main(void *data);

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
    return EXIT_FAILURE;
  }
  window.set_window_title("RPS version " + get_version() + " by Eugene Hopkinson");

  emscripten_set_main_loop_arg(&loop_select_difficulty, nullptr, 0, true);      // loop function, user data, FPS (0 to use browser requestAnimationFrame mechanism), simulate infinite loop

  std::unreachable();                                                           // execution never returns to this point
}


void loop_select_difficulty(void *data) {
  // TODO
}

void loop_main(void *data) {
  // TODO
}
