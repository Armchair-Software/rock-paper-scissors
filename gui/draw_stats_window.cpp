#include "draw_stats_window.h"
#include <imgui/imgui.h>
#include "game_state.h"

namespace gui {

void draw_stats_window(game_state const &state) {
  /// Draw a window containing the game statistics
  ImGui::SetNextWindowSize(vec2f{0, -FLT_MIN}, ImGuiCond_Always);
  if(ImGui::Begin("Stats", nullptr)) {
    ImGui::PushItemWidth(50);
    ImGui::InputInt("Rounds played", const_cast<int*>(reinterpret_cast<int const*>(&state.rounds_played)), 0, 0, ImGuiInputTextFlags_ReadOnly); // imgui expects a signed int - the input is read-only and in our expected range this is safe
    if(state.rounds_played != 0) {
      ImGui::InputInt(("Wins (" + std::to_string((state.wins * 100) / state.rounds_played) + "%)").c_str(), const_cast<int*>(reinterpret_cast<int const*>(&state.rounds_played)), 0, 0, ImGuiInputTextFlags_ReadOnly);
      ImGui::InputInt(("Draws (" + std::to_string((state.draws * 100) / state.rounds_played) + "%)").c_str(), const_cast<int*>(reinterpret_cast<int const*>(&state.draws)), 0, 0, ImGuiInputTextFlags_ReadOnly);
      auto const losses{state.rounds_played - state.wins - state.draws};
      ImGui::InputInt(("Losses (" + std::to_string((losses * 100) / state.rounds_played) + "%)").c_str(), const_cast<int*>(reinterpret_cast<int const*>(&losses)), 0, 0, ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::PopItemWidth();
  }
  ImGui::End();
}

}
