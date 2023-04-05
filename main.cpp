#include <boost/throw_exception.hpp>
#include <iostream>
#include <emscripten.h>
#include "get_version.h"


#ifdef BOOST_NO_EXCEPTIONS
void boost::throw_exception(std::exception const & e) {
  /// Custom exception replacement function when boost exceptions are disabled
  std::cout << "ERROR: Boost would have thrown exception: " << e.what() << std::endl;
  abort();
}
#endif // BOOST_NO_EXCEPTIONS

static void loop_main(void *data);

[[noreturn]] auto main()->int {
  // TODO
  emscripten_set_main_loop_arg(&loop_main, nullptr, 0, true);                    // loop function, user data, FPS (0 to use browser requestAnimationFrame mechanism), simulate infinite loop

  std::unreachable();                                                           // execution never returns to this point
}


void loop_main(void *data) {
  // TODO
}
