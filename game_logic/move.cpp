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

}
