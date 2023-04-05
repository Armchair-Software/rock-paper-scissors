#ifndef RENDER_TEXTURE_MANAGER_H_INCLUDED
#define RENDER_TEXTURE_MANAGER_H_INCLUDED

#include <GLES/gl.h>
#include <vectorstorm/vector/vector2.h>
#include <vectorstorm/aabb/aabb2.h>

namespace render {

class texture_manager {
public:
  GLuint texture_atlas = 0;                                                     // OpenGL handle to the texture atlas
  vec2ui texture_size;;                                                         // Size of the texture, determined at load time

  enum class texture_id : unsigned int {
    /// Hard-coded sub-texture identities - in a real program these would be loaded dynamically
    hand_rock,
    hand_paper,
    hand_scissors,
    icon_rock,
    icon_paper,
    icon_scissors,
  };
  std::unordered_map<texture_id, aabb2f> subtextures;                           // map of texture ID to sub-texture region in texels (axis-aligned bounding box)
  static constexpr vec2ui subtexture_size{128, 128};                            // size of sub-textures in the texture atlas, in px

  texture_manager();
  ~texture_manager();
};

}

#endif // RENDER_TEXTURE_MANAGER_H_INCLUDED
