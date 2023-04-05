#ifndef GUI_DRAW_IMAGE_BUTTON_H_INCLUDED
#define GUI_DRAW_IMAGE_BUTTON_H_INCLUDED

#include <string>
#include "render/texture_manager.h"

struct game_state;

namespace gui {

bool draw_image_button(std::string const &button_name, game_state &state, render::texture_manager::texture_id texture_id);

}

#endif // GUI_DRAW_IMAGE_BUTTON_H_INCLUDED
