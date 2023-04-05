#include "scene.h"
#include <imgui/imgui.h>
#include <vectorstorm/lerp.h>
#include "game_state.h"

namespace render {

void scene::update() {
  /// Tick over the animation for the background scene
  hand_travel_opponent = lerp(hand_travel_opponent, hand_target_opponent, 0.2f); // smoothly slide the hands in and out of the frame
  hand_travel_player = lerp(hand_travel_player, hand_target_player, 0.15f);
  colour_background = colour_background.lerp(0.1f, colour_background_target);   // smoothly fade the background colour to target
}

void scene::draw(game_state &state) const {
  /// Render the background scene
  auto const &viewport_size{ImGui::GetMainViewport()->Size};                    // cache viewport size
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
  ImGui::SetNextWindowSize(viewport_size);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, colour_background);                  // set window background colour
  ImGui::Begin("scene_window", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
  ImGui::PopStyleColor();
  ImGui::PopStyleVar(3);

  auto const &opponent_texture_id{get_hand_texture_id(opponent_move)};
  auto const &player_texture_id{get_hand_texture_id(player_move)};


  auto scale{static_cast<unsigned int>(std::floor(viewport_size.x / (texture_manager::subtexture_size.x * 0.85f)))};
  auto const image_size{static_cast<vec2f>(texture_manager::subtexture_size * scale)};
  constexpr float vertical_offset{0.5f};

  auto const hand_start_anchor_opponent{vec2f{-image_size.x, viewport_size.y * -0.25f}}; // quarter of the way above the screen
  auto const hand_start_anchor_player{vec2f{viewport_size.x + image_size.x, viewport_size.y * 0.25f}}; // quarter of the way down the screen, right edge
  auto const hand_start_coord_opponent{vec2f{hand_start_anchor_opponent.x, hand_start_anchor_opponent.y - (image_size.y * vertical_offset)}};
  auto const hand_start_coord_player{vec2f{hand_start_anchor_player.x - image_size.x, hand_start_anchor_player.y - (image_size.y * vertical_offset)}};

  auto const hand_destination_anchor_opponent{vec2f{0.0f, viewport_size.y * 0.25f}}; // quarter of the way down the screen
  auto const hand_destination_anchor_player{vec2f{viewport_size.x, viewport_size.y * 0.75f}}; // quarter of the way up the screen, right edge
  auto const hand_destination_coord_opponent{vec2f{hand_destination_anchor_opponent.x, hand_destination_anchor_opponent.y - (image_size.y * vertical_offset)}};
  auto const hand_destination_coord_player{vec2f{hand_destination_anchor_player.x - image_size.x, hand_destination_anchor_player.y - (image_size.y * vertical_offset)}};

  auto const hand_coord_opponent{hand_start_coord_opponent.lerp(hand_travel_opponent, hand_destination_coord_opponent)};
  auto const hand_coord_player{hand_start_coord_player.lerp(hand_travel_player, hand_destination_coord_player)};

  ImGui::SetCursorPos(hand_coord_opponent);
  ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(state.textures.texture_atlas)), // safe path to convert integer OpenGL handle to a void pointer
               image_size,                                                      // image size
               state.textures.subtextures[opponent_texture_id].min,             // uv0
               state.textures.subtextures[opponent_texture_id].max,             // uv1
               ImVec4(1, 1, 1, 1),                                              // tint colour
               ImVec4(0, 0, 0, 0));                                             // border colour

  ImGui::SetCursorPos(hand_coord_player);
  auto const uv0_mirror{vec2f{state.textures.subtextures[player_texture_id].max.x, state.textures.subtextures[player_texture_id].min.y}};
  auto const uv1_mirror{vec2f{state.textures.subtextures[player_texture_id].min.x, state.textures.subtextures[player_texture_id].max.y}};
  ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(state.textures.texture_atlas)), // safe path to convert integer OpenGL handle to a void pointer
               image_size,                                                      // image size
               uv0_mirror,                                                      // uv0
               uv1_mirror,                                                      // uv1
               ImVec4(1, 1, 1, 1),                                              // tint colour
               ImVec4(0, 0, 0, 0));                                             // border colour

  ImGui::End();
}

}
