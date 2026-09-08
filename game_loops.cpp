#include "game_loops.h"
#include <imgui/imgui.h>
#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>
#include "game_state.h"
#include "render/scene.h"
#include "gui/gui.h"
#include "gui/draw_stats_window.h"
#include "gui/draw_image_button.h"
#include "game_logic/game_logic_base.h"

void loop_select_difficulty(void *data) {
  /// UI loop to prompt user to select game difficulty
  auto &state{*static_cast<game_state*>(data)};
  gui::frame_begin();
  state.scene.update();                                                         // update the scene state
  state.scene.draw(state);                                                      // draw the scene behind the GUI

  ImGui::SetNextWindowSize(vec2f{0, -FLT_MIN}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, vec2f{0.5f, 0.5f}); // set next window position in the centre of the frame, use pivot=(0.5f,0.5f) to center on given point, etc.
  if(ImGui::Begin("Start game", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
    ImGui::TextUnformatted("Rounds to play");
    ImGui::SetNextItemWidth(-FLT_MIN);                                          // stretch next input to full width
    if(ImGui::InputInt("##rounds", reinterpret_cast<int*>(&state.rounds_to_play), 1, 5)) { // input for how many rounds to play - imgui expects a signed int - with the limited range, treating our unsigned int as signed is safe
      state.rounds_to_play = std::clamp(state.rounds_to_play, 1u, 100u);        // keep the numbers within a reasonable range
    }

    ImGui::TextUnformatted("Difficulty");
    magic_enum::enum_for_each<game_logic::difficulty_type>([&](game_logic::difficulty_type difficulty) {
      std::string difficulty_name{magic_enum::enum_name(difficulty)};           // derive difficulty name from enum name
      difficulty_name[0] = static_cast<char>(std::toupper(difficulty_name[0])); // capitalise first letter of the difficulty name
      if(ImGui::Button(difficulty_name.c_str(), vec2f{200, 40})) {              // difficulty select button
        state.logic.set_difficulty(difficulty);                                 // set the difficulty in the game logic engine
        state.next_loop = game_state::next_loop_type::player_move;              // proceed to the player's first turn
      }
      if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled | ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip(state.logic.difficulty_description(difficulty).c_str(), "%s"); // set tooltip to describe the difficulty relevant to the active logic engine
      }
    });
  }
  ImGui::End();

  gui::frame_end(state.window);
  state.dispatch_next_loop();
}

void loop_player_move(void *data) {
  /// UI loop to prompt the player to make a move
  auto &state{*static_cast<game_state*>(data)};
  gui::frame_begin();
  state.scene.update();                                                         // update the scene state
  state.scene.draw(state);                                                      // draw the scene behind the GUI

  ImGui::SetNextWindowSize(vec2f{0, -FLT_MIN}, ImGuiCond_Always);
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, vec2f{0.5f, 0.5f}); // set next window position in the centre of the frame, use pivot=(0.5f,0.5f) to center on given point, etc.
  if(ImGui::Begin("Choose your move", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
    magic_enum::enum_for_each<game_logic::move_type>([&](game_logic::move_type move) {
      std::string move_name{magic_enum::enum_name(move)};                       // derive move name from enum name
      move_name[0] = static_cast<char>(std::toupper(move_name[0]));             // capitalise first letter of the name
      if(gui::draw_image_button(move_name, state, game_logic::get_icon_texture_id(move))) { // image button for each move option
        state.logic.process();                                                  // calculate the next move
        state.logic.player_move(move);                                          // declare the player move

        state.next_loop = game_state::next_loop_type::opponent_move;            // opponent's turn
      }
      if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled | ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip(game_logic::describe_move(move).c_str(), "%s");       // set tooltip to describe the behaviour of the move
      }
      ImGui::SameLine();
    });
  }
  ImGui::End();

  gui::draw_stats_window(state);                                                // draw the game statistics window
  gui::frame_end(state.window);
  state.dispatch_next_loop();
}

void loop_opponent_move(void *data) {
  /// UI loop to show the player the result of the last round
  auto &state{*static_cast<game_state*>(data)};
  gui::frame_begin();
  state.scene.update();                                                         // update the scene state
  state.scene.draw(state);                                                      // draw the scene behind the GUI

  ImGui::SetNextWindowSize(vec2f{0, -FLT_MIN}, ImGuiCond_Always);
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // set next window position in the centre of the frame
  if(ImGui::Begin("Showdown!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
    ImGui::TextUnformatted(("You played: " + std::string{magic_enum::enum_name(state.logic.get_last_player_move())} + "...").c_str());
    ImGui::TextUnformatted(("Opponent played: " + std::string{magic_enum::enum_name(state.logic.get_last_opponent_move())} + "!").c_str());

    ImGui::Spacing();
    auto verdict{state.logic.get_last_verdict()};
    ImGui::TextUnformatted(("You " + std::string{magic_enum::enum_name(verdict)} + ".").c_str());

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    if(state.rounds_played < state.rounds_to_play) {                            // only show the next round button if we have more rounds to play
      if(ImGui::Button("Next round", vec2f{200, 40})) {
        state.next_loop = game_state::next_loop_type::player_move;              // proceed to the player's turn
      }
    } else {
      std::ostringstream resultss;
      resultss << "You won " << state.wins << " rounds and lost " << state.rounds_played - state.wins - state.draws << ".\n";
      ImGui::TextUnformatted(resultss.str().c_str());
      if(state.wins > state.rounds_played - state.wins - state.draws) {
        ImGui::TextUnformatted("Congratulations!");
      } else {
        ImGui::TextUnformatted("Better luck next time.");
      }
      ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    }
    if(ImGui::Button("End game", vec2f{200, 20})) {
      state.next_loop = game_state::next_loop_type::select_difficulty;          // proceed to the difficulty select loop
    }
  }
  ImGui::End();

  gui::draw_stats_window(state);                                                // draw the game statistics window
  gui::frame_end(state.window);
  state.dispatch_next_loop();
}
