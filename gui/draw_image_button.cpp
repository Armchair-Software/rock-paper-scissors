#include "draw_image_button.h"
#include <imgui/imgui.h>
#include "game_state.h"

namespace gui {

bool draw_image_button(std::string const &button_name, game_state &state, render::texture_manager::texture_id texture_id) {
  /// Convenience wrapper for image button, referencing a sub-texture from the texture atlas
  return ImGui::ImageButton(button_name.c_str(),
                            state.textures.texture_atlas,                       // safe path to convert integer OpenGL handle to a void pointer
                            vec2f{128.0f, 128.0f},                              // button size
                            state.textures.subtextures[texture_id].min,         // uv0
                            state.textures.subtextures[texture_id].max,         // uv1
                            ImVec4(0, 0, 0, 0),                                 // background colour
                            ImVec4(1, 1, 1, 1));                                // tint colour
}

}
