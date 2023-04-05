#include <boost/throw_exception.hpp>
#include <iostream>
#include <emscripten.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <magic_enum.hpp>
#include "render/window.h"
#include "game_logic/offline.h"
#include "gui/gui.h"
#include "get_version.h"

#ifdef BOOST_NO_EXCEPTIONS
void boost::throw_exception(std::exception const & e) {
  /// Custom exception replacement function when boost exceptions are disabled
  std::cout << "ERROR: Boost would have thrown exception: " << e.what() << std::endl;
  abort();
}
#endif // BOOST_NO_EXCEPTIONS


static void loop_select_difficulty(void *data);
static void loop_player_move(void *data);
static void loop_opponent_move(void *data);

struct game_state {
  render::window &window;
  game_logic::game_logic_base &logic;
  unsigned int rounds_to_play = 5;
  unsigned int rounds_played = 0;
  unsigned int wins = 0;
  unsigned int draws = 0;
};

[[noreturn]] auto main()->int {                                                 // noreturn here is not standards-compliant, but is appropriate for emscripten with a main loop
  render::window window;
  window.set_window_title("Loading: RPS version " + get_version());
  if(!window.init()) {
    std::cerr << "ERROR: Failed to create a window, cannot continue." << std::endl;
    EM_ASM(
      if(confirm("Error - unable to create a graphics window.  Please check that WebGL 2.0 is supported by your browser, and is enabled.  Press OK to visit a validation page.")) {
        window.location.replace("https://get.webgl.org/webgl2/");
      }
    );
    std::abort();
  }
  window.set_window_title("RPS version " + get_version() + " by Eugene Hopkinson");

  gui::init(window);                                                            // set up the GUI

  game_logic::offline logic;                                                    // game logic: offline local AI

  game_state state{                                                             // package references to game state for the loops
    window,
    logic
  };
  emscripten_set_main_loop_arg(&loop_select_difficulty, &state, 0, true);       // loop function, user data, FPS (0 to use browser requestAnimationFrame mechanism), simulate infinite loop

  std::unreachable();                                                           // execution never returns to this point
}


void loop_select_difficulty(void *data) {
  /// UI loop to prompt user to select game difficulty
  auto &state{*static_cast<game_state*>(data)};

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  bool move_to_main_loop{false};

  ImGui::SetNextWindowSize(vec2f{0, -FLT_MIN}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, vec2f{0.5f, 0.5f}); // set next window position in the centre of the frame, use pivot=(0.5f,0.5f) to center on given point, etc.
  if(ImGui::Begin("Start game", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
    ImGui::TextUnformatted("Rounds to play");
    ImGui::SetNextItemWidth(-FLT_MIN);                                          // stretch next input to full width
    if(ImGui::InputInt("##rounds", reinterpret_cast<int*>(&state.rounds_to_play), 1, 5)) { // input for how many rounds to play
      state.rounds_to_play = std::clamp(state.rounds_to_play, 1u, 100u);        // keep the numbers within a reasonable range
    }

    ImGui::TextUnformatted("Difficulty");
    magic_enum::enum_for_each<game_logic::difficulty_type>([&](game_logic::difficulty_type difficulty) {
      std::string difficulty_name{magic_enum::enum_name(difficulty)};           // derive difficulty name from enum name
      difficulty_name[0] = static_cast<char>(std::toupper(difficulty_name[0])); // capitalise first letter of the difficulty name
      if(ImGui::Button(difficulty_name.c_str(), vec2f{200, 40})) {              // difficulty select button
        state.logic.set_difficulty(difficulty);                                 // set the difficulty in the game logic engine
        move_to_main_loop = true;                                               // proceed to the main loop
      }
      if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled | ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip(state.logic.difficulty_description(difficulty).c_str(), "%s"); // set tooltip to describe the difficulty relevant to the active logic engine
      }
    });
  }
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(state.window.glfw_window);

  if(move_to_main_loop) {
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(&loop_player_move, &state, 0, true);           // loop function, user data, FPS (0 to use browser requestAnimationFrame mechanism), simulate infinite loop
  }
}

void loop_player_move(void *data) {
  auto &state{*static_cast<game_state*>(data)};

  enum class next_loop_type {
    continue_this,
    opponent_move,
    select_difficulty
  } next_loop = next_loop_type::continue_this;                                  // where to direct logic flow after this loop is finished

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowSize(vec2f{0, -FLT_MIN}, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, vec2f{0.5f, 0.5f}); // set next window position in the centre of the frame, use pivot=(0.5f,0.5f) to center on given point, etc.
  if(ImGui::Begin("Choose your move", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
    magic_enum::enum_for_each<game_logic::move_type>([&](game_logic::move_type move) {
      std::string move_name{magic_enum::enum_name(move)};                       // derive move name from enum name
      move_name[0] = static_cast<char>(std::toupper(move_name[0]));             // capitalise first letter of the name
      if(ImGui::Button(move_name.c_str(), vec2f{100, 40})) {                    // select move button
        state.logic.process();                                                  // calculate the next move
        state.logic.player_move(move);                                          // declare the player move
        next_loop = next_loop_type::opponent_move;                              // opponent's turn
      }
      if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled | ImGuiHoveredFlags_DelayNormal)) {
        ImGui::SetTooltip(game_logic::describe_move(move).c_str(), "%s");       // set tooltip to describe the behaviour of the move
      }
      ImGui::SameLine();
    });
  }
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(state.window.glfw_window);

  switch(next_loop) {
  case next_loop_type::opponent_move:
    {
      ++state.rounds_played;
      auto verdict{state.logic.get_last_verdict()};
      switch(verdict) {
      case game_logic::verdict_type::win:
        ++state.wins;
        break;
      case game_logic::verdict_type::draw:
        ++state.draws;
        break;
      case game_logic::verdict_type::lose:
        // losses are calculated automatically from total, wins and draws
        break;
      }
      emscripten_cancel_main_loop();
      emscripten_set_main_loop_arg(&loop_opponent_move, &state, 0, true);
    }
    break;
  case next_loop_type::select_difficulty:
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(&loop_select_difficulty, &state, 0, true);
    break;
  case next_loop_type::continue_this:
    // continue this loop
  }
}

void loop_opponent_move(void *data) {
  auto &state{*static_cast<game_state*>(data)};

  enum class next_loop_type {
    continue_this,
    player_move,
    select_difficulty
  } next_loop = next_loop_type::continue_this;                                  // where to direct logic flow after this loop is finished

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // set next window position in the centre of the frame, use pivot=(0.5f,0.5f) to center on given point, etc.
  if(ImGui::Begin("Showdown", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
    auto opponent_move{state.logic.get_last_opponent_move()};
    ImGui::TextUnformatted(("Your opponent's move: " + std::string{magic_enum::enum_name(opponent_move)} + "!").c_str());
    auto verdict{state.logic.get_last_verdict()};
    ImGui::TextUnformatted(("Result: " + std::string{magic_enum::enum_name(verdict)}).c_str());
    std::stringstream totalss;
    auto const losses{state.rounds_played - state.wins - state.draws};
    totalss << "Total: " << state.rounds_played << " rounds with "
            << state.wins  << " wins ("   << (state.wins  * 100) / state.rounds_played << "%), "
            << state.draws << " draws ("  << (state.draws * 100) / state.rounds_played << "%), "
            << losses      << " losses (" << (losses      * 100) / state.rounds_played << "%)";
    ImGui::TextUnformatted(totalss.str().c_str());

    if(state.rounds_played < state.rounds_to_play) {                            // only show the next round button if we have more rounds to play
      if(ImGui::Button("Next round")) {
        next_loop = next_loop_type::player_move;                                // proceed to the player's turn
      }
    }
    if(ImGui::Button("End game")) {
      next_loop = next_loop_type::select_difficulty;                            // proceed to the difficulty select loop
    }
  }
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(state.window.glfw_window);

  switch(next_loop) {
  case next_loop_type::player_move:
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(&loop_player_move, &state, 0, true);
    break;
  case next_loop_type::select_difficulty:
    state.rounds_played = 0;
    state.wins = 0;
    state.draws = 0;
    emscripten_cancel_main_loop();
    emscripten_set_main_loop_arg(&loop_select_difficulty, &state, 0, true);
    break;
  case next_loop_type::continue_this:
    // continue this loop
  }
}
