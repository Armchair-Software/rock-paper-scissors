#include "move.h"

namespace game_logic {

std::string describe_move(move_type move) {
  /// Simple text description of the behaviour of each move, for tooltips etc
  #pragma GCC diagnostic push
  #pragma GCC diagnostic error "-Wswitch"                                       // enforce exhaustive switch here
  switch(move) {
  #pragma GCC diagnostic pop
  case move_type::rock:
    return "Rock breaks scissors.";
  case move_type::paper:
    return "Paper wraps rock.";
  case move_type::scissors:
    return "Scissors cut paper.";
  }                                                                             // no default case, to enforce exhaustive switch
  std::unreachable();
}

render::texture_manager::texture_id get_icon_texture_id(move_type move) {
  /// Match up subtexture IDs in the texture atlas with moves
  // normally this sort of data would be loaded rather than hardcoded, but we're going for brevity here
  #pragma GCC diagnostic push
  #pragma GCC diagnostic error "-Wswitch"                                       // enforce exhaustive switch here
  switch(move) {
  #pragma GCC diagnostic pop
  case move_type::rock:
    return render::texture_manager::texture_id::icon_rock;
  case move_type::paper:
    return render::texture_manager::texture_id::icon_paper;
  case move_type::scissors:
    return render::texture_manager::texture_id::icon_scissors;
  }                                                                             // no default case, to enforce exhaustive switch
  std::unreachable();
}

render::texture_manager::texture_id get_hand_texture_id(move_type move) {
  /// Match up subtexture IDs in the texture atlas with moves
  // ditto re. hardcoding above
  #pragma GCC diagnostic push
  #pragma GCC diagnostic error "-Wswitch"                                       // enforce exhaustive switch here
  switch(move) {
  #pragma GCC diagnostic pop
  case move_type::rock:
    return render::texture_manager::texture_id::hand_rock;
  case move_type::paper:
    return render::texture_manager::texture_id::hand_paper;
  case move_type::scissors:
    return render::texture_manager::texture_id::hand_scissors;
  }                                                                             // no default case, to enforce exhaustive switch
  std::unreachable();
}

}
