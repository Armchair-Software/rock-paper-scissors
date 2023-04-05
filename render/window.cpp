#include "window.h"
#include <iostream>
#include <emscripten/val.h>

namespace render {

EM_BOOL callback_window_resize([[maybe_unused]] int event_type, const EmscriptenUiEvent *event, void *data) { // event_type == EMSCRIPTEN_EVENT_RESIZE, docs: https://emscripten.org/docs/api_reference/html5.h.html#id16
  /// Handle a browser window resize event
  auto &this_window{*static_cast<window*>(data)};
  this_window.document_body_size.assign(static_cast<unsigned int>(event->documentBodyClientWidth), static_cast<unsigned int>(event->documentBodyClientHeight));
  this_window.window_inner_size.assign(static_cast<unsigned int>(event->windowInnerWidth), static_cast<unsigned int>(event->windowInnerHeight));
  this_window.window_outer_size.assign(static_cast<unsigned int>(event->windowOuterWidth), static_cast<unsigned int>(event->windowOuterHeight));
  emscripten_get_canvas_element_size("#canvas", &this_window.canvas_size.x, &this_window.canvas_size.y); // update the cached canvas size
  this_window.device_pixel_ratio = emscripten::val::global("window")["devicePixelRatio"].as<float>(); // query device pixel ratio using JS
  this_window.update_viewport_size();                                           // update the viewport
  return false;                                                                 // allow other handlers to handle this event also
}

bool window::init() {
  /// Initialise the window and graphics context
  std::cout << "render::window: Initialising graphics..." << std::endl;
  if(glfwInit() != GLFW_TRUE) {                                                 // initialise the opengl window
    std::cerr << "render::window: ERROR: Graphics initialisation failed!  Cannot continue." << std::endl;
    return false;
  }

  auto callback_error_func = [](int error, char const *description){            // set the error callback before any GL operations
    std::cerr << "ERROR: GLFW: " << error << ": " << description << std::endl;
  };
  callback_error = std::bind(callback_error_func, std::placeholders::_1, std::placeholders::_2); // bind the lambda to create a function in the form GLFW expects
  glfwSetErrorCallback(callback_error.target<void(int, char const*)>());        // pass the bound target to GLFW to set the callback

  // find out about the initial canvas size and the current window and doc sizes
  emscripten_get_canvas_element_size("#canvas", &canvas_size.x, &canvas_size.y);
  document_body_size.assign(emscripten::val::global("document")["body"]["clientWidth"].as<unsigned int>(),
                            emscripten::val::global("document")["body"]["clientHeight"].as<unsigned int>());
  window_inner_size.assign( emscripten::val::global("window")["innerWidth"].as<unsigned int>(),
                            emscripten::val::global("window")["innerHeight"].as<unsigned int>());
  window_outer_size.assign( emscripten::val::global("window")["outerWidth"].as<unsigned int>(),
                            emscripten::val::global("window")["outerHeight"].as<unsigned int>());
  device_pixel_ratio = emscripten::val::global("window")["devicePixelRatio"].as<float>(); // query device pixel ratio using JS
  viewport_size = window_inner_size;

  emscripten_set_resize_callback(
    EMSCRIPTEN_EVENT_TARGET_WINDOW,                                             // target = EMSCRIPTEN_EVENT_TARGET_WINDOW to get resize events from the Window object
    this,                                                                       // userData
    false,                                                                      // useCapture
    callback_window_resize                                                      // callback
  );

  // set up the main window's hints in advance
  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);                         // forward compat disables all deprecated functions - we don't want that

  #ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  #endif // NDEBUG
  glfw_window = glfwCreateWindow(canvas_size.x,                                 // use initial canvas size
                                 canvas_size.y,
                                 window_title.c_str(),                          // window title
                                 nullptr,                                       // monitor to use fullscreen, NULL here means run windowed - we always do under emscripten
                                 nullptr);                                      // the context to share with, see http://stackoverflow.com/a/17792242/1678468

  if(!glfw_window) {                                                            // exit if this didn't work
    std::cerr << "render::window: ERROR: Failed to open window!  Cannot continue." << std::endl;
    return false;
  }
  glfwMakeContextCurrent(glfw_window);

  GLint antialiasing_buffers, depthbits, stencilbits, maxverts, maxindices, maxtexturelayers, maxtexunits, antialiasing_samples, antialiasing_max_samples, antialiasing_max_samples_fbo;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &antialiasing_buffers);
  glGetIntegerv(GL_SAMPLES, &antialiasing_samples);
  glGetIntegerv(GL_SAMPLES, &antialiasing_max_samples);
  glGetIntegerv(GL_MAX_SAMPLES, &antialiasing_max_samples_fbo);                 // use samples extension, for use with FBO - see https://www.opengl.org/wiki/GL_EXT_framebuffer_multisample
  glGetIntegerv(GL_DEPTH_BITS, &depthbits);
  glGetIntegerv(GL_STENCIL_BITS, &stencilbits);
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxverts);
  glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxindices);
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_size_max);
  glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxtexturelayers);
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxtexunits);
  glGetIntegerv(GL_MAX_VIEWPORT_DIMS, max_viewport_size);

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
  glClearStencil(0);                                                            // value with which to clear the stencil buffer
  glStencilMask(0xFF);                                                          // enable writing to all bits of the stencil buffer (when stencil test is enabled)

  update_viewport_size();

  initialised = true;
  std::cout << "render::window: Render window created" << std::endl;

  return true;
}

void window::shutdown() {
  /// Close the display window and free graphics resources
  initialised = false;
}

void window::set_window_title(std::string const &new_window_title) {
  /// Update the title of the displayed window
  window_title = new_window_title;

  if(initialised) {
    glfwSetWindowTitle(glfw_window, window_title.c_str());
  }
}

void window::update_viewport_size() {
  /// Update the viewport size based on what we know about the window at present
  vec2f viewport_size_scaled = static_cast<vec2f>(document_body_size) * device_pixel_ratio; // use document body size - make sure css is configured for html and body to take up 100% of the browser window height
  viewport_size.assign(static_cast<int>(std::round(viewport_size_scaled.x)), static_cast<int>(std::round(viewport_size_scaled.y))); // round the scaled size
  viewport_size = std::min(viewport_size, max_viewport_size);                   // clamp viewport size to permitted max

  glfwSetWindowSize(glfw_window, viewport_size.x, viewport_size.y);
  glViewport(0, 0, viewport_size.x, viewport_size.y);
  glScissor(0, 0, viewport_size.x, viewport_size.y);
  emscripten_get_canvas_element_size("#canvas", &canvas_size.x, &canvas_size.y);

  if(callback_refresh) callback_refresh(*this);
}

vec2i const &window::get_viewport_size() const {
  return viewport_size;
}

void window::set_refresh_callback(std::function<void(window &this_window)> new_callback) {
  callback_refresh = new_callback;
}

}
