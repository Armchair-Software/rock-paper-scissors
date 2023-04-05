#include "texture_manager.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLES/glext.h>
#include <png++/png.hpp>
#include <vectorstorm/vector/vector4.h>

namespace render {

texture_manager::texture_manager() {
  /// Default constructor: initialise the texture atlas
  glGenTextures(1, &texture_atlas);                                             // generate texture atlas

  if(texture_atlas == 0) {
    std::cerr << "ERROR: Block editor: Failed to allocate canvas texture, cannot continue" << std::endl;
    abort();                                                                    // in lieu of exceptions
  }

  png::image<png::rgba_pixel> png_data{"textureatlas.png"};                     // load texture atlas from the embedded emscripten filesystem
  //png::image<png::rgba_pixel> png_data{"temp.png"};                     // load texture atlas from the embedded emscripten filesystem
  texture_size.assign(png_data.get_width(), png_data.get_height());             // cache the texture size
  std::cout << "Texture atlas: PNG decoded, size " << texture_size << std::endl;

  // configure texture parameters
  glBindTexture(GL_TEXTURE_2D, texture_atlas);                                  // bind the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);            // for rectangular pixels
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD,   0);                      // minimum mipmap detail (highest resolution)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD,   0);                      // maximum mipmap detail (lowest resolution)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);                      // maximum mipmap level
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);          // anisotropic filtering

  // create the texture but don't assign content
  glTexStorage2D(GL_TEXTURE_2D,                                                 // target
                 1,                                                             // mipmap levels, min 1
                 GL_RGBA8,                                                      // sized internal format
                 static_cast<GLsizei>(texture_size.x),                          // width
                 static_cast<GLsizei>(texture_size.y));                         // height

  std::vector<vec4<uint8_t>> pixel_data(texture_size.x * texture_size.y);       // create a temporary buffer to load the PNG data to, prior to upload to OpenGL
  for(unsigned int y = 0; y != texture_size.y; ++y) {                           // manual swizzle is needed because the format would be GL_ABGR, which can be loaded by glTex with GL_EXT_abgr, but the extension not available on WebGL
    for(unsigned int x = 0; x != texture_size.x; ++x) {
      auto const &that_pixel{png_data[y][x]};
      auto &this_pixel{pixel_data[(y * texture_size.x) + x]};
      this_pixel.assign(that_pixel.red, that_pixel.green, that_pixel.blue, that_pixel.alpha);
      if(this_pixel != vec4<uint8_t>{0, 0, 0, 0}) std::cout << "Texture atlas: Pixel " << this_pixel << std::endl;
    }
  }
  glBindTexture(GL_TEXTURE_2D, 0);                                              // release the texture
  glBindTexture(GL_TEXTURE_2D, texture_atlas);                                  // bind the texture

  // upload the texture data to OpenGL
  glTexSubImage2D(GL_TEXTURE_2D,                                                // target
                  0,                                                            // mipmap level
                  0,                                                            // xoffset
                  0,                                                            // yoffset
                  static_cast<GLsizei>(texture_size.x),                         // width
                  static_cast<GLsizei>(texture_size.y),                         // height
                  GL_RGBA,                                                      // format: GL_ABGR (via extension, needed for png++ load) or GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, GL_STENCIL_INDEX
                  GL_UNSIGNED_BYTE,                                             // data type
                  pixel_data.data());                                           // data in memory

  glBindTexture(GL_TEXTURE_2D, 0);                                              // release the texture

  // Hard-coded sub-texture locations - in a real program these would be loaded dynamically, but for simplicity we specify them manually here
  subtextures.emplace(texture_id::hand_rock,     aabb2f{0.0f, 0.0f, 0.25f, 0.25f} + vec2f{0.00f, 0.00f});
  subtextures.emplace(texture_id::hand_paper,    aabb2f{0.0f, 0.0f, 0.25f, 0.25f} + vec2f{0.00f, 0.25f});
  subtextures.emplace(texture_id::hand_scissors, aabb2f{0.0f, 0.0f, 0.25f, 0.25f} + vec2f{0.00f, 0.50f});
  subtextures.emplace(texture_id::icon_rock,     aabb2f{0.0f, 0.0f, 0.25f, 0.25f} + vec2f{0.25f, 0.00f});
  subtextures.emplace(texture_id::icon_paper,    aabb2f{0.0f, 0.0f, 0.25f, 0.25f} + vec2f{0.25f, 0.25f});
  subtextures.emplace(texture_id::icon_scissors, aabb2f{0.0f, 0.0f, 0.25f, 0.25f} + vec2f{0.25f, 0.50f});
}

texture_manager::~texture_manager() {
  /// Default destructor: clean up the texture
  if(texture_atlas != 0) {
    glDeleteTextures(1, &texture_atlas);
    texture_atlas = 0;
  }
}

}
