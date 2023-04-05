#include "gui.h"
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "vectorstorm/vector/vector3.h"
#include "render/window.h"

namespace gui {

void init(render::window &window) {
  /// Set up Dear ImGUI
  #ifndef NDEBUG
    IMGUI_CHECKVERSION();
  #endif // NDEBUG
  ImGui::CreateContext();
  ImGuiIO &imgui_io{ImGui::GetIO()};
  imgui_io.IniFilename = nullptr;                                               // disable saving settings to disk
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;                 // don't attempt to change mouse cursors
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;                   // enable keyboard controls
  //imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;                    // enable gamepad controls

  ImGui::StyleColorsDark();                                                     // imgui style
  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowRounding = 3;
  style.FrameRounding = 2;
  style.GrabRounding = 1;

  std::vector<vec3ui> palette{{                                                 // orange
    vec3ui{  0,   0, 0},
    vec3ui{ 16,   8, 0},
    vec3ui{ 32,  16, 0},
    vec3ui{ 52,  24, 0},
    vec3ui{ 69,  36, 0},
    vec3ui{ 85,  44, 0},
    vec3ui{101,  52, 0},
    vec3ui{117,  60, 0},
    vec3ui{138,  69, 0},
    vec3ui{154,  77, 0},
    vec3ui{170,  85, 0},
    vec3ui{186,  93, 0},
    vec3ui{203, 105, 0},
    vec3ui{223, 113, 0},
    vec3ui{239, 121, 0},
    vec3ui{255, 130, 0},
  }};

  auto get_float_colour = [&](unsigned int index, float alpha = 1.0f) {
    vec3f const colour_rgb{static_cast<vec3f>(palette[index]) / 255.0f};
    return vec4f(colour_rgb.r, colour_rgb.g, colour_rgb.b, alpha);
  };
  ImVec4 *colors{ImGui::GetStyle().Colors};

  // lightness 15
  colors[ImGuiCol_FrameBgHovered]         = get_float_colour(15, 0.40f);
  colors[ImGuiCol_FrameBgActive]          = get_float_colour(15, 0.67f);
  colors[ImGuiCol_CheckMark]              = get_float_colour(15);
  colors[ImGuiCol_SliderGrabActive]       = get_float_colour(15);
  colors[ImGuiCol_Button]                 = get_float_colour(15, 0.40f);
  colors[ImGuiCol_ButtonHovered]          = get_float_colour(15);
  colors[ImGuiCol_Header]                 = get_float_colour(15, 0.31f);
  colors[ImGuiCol_HeaderHovered]          = get_float_colour(15, 0.80f);
  colors[ImGuiCol_HeaderActive]           = get_float_colour(15);
  colors[ImGuiCol_ResizeGrip]             = get_float_colour(15, 0.20f);
  colors[ImGuiCol_ResizeGripHovered]      = get_float_colour(15, 0.67f);
  colors[ImGuiCol_ResizeGripActive]       = get_float_colour(15, 0.95f);
  colors[ImGuiCol_TabHovered]             = get_float_colour(15, 0.80f);
  colors[ImGuiCol_DockingPreview]         = get_float_colour(15, 0.70f);
  colors[ImGuiCol_TextSelectedBg]         = get_float_colour(15, 0.35f);
  colors[ImGuiCol_NavHighlight]           = get_float_colour(15);

  // lightness 14
  colors[ImGuiCol_SliderGrab]             = get_float_colour(14);

  // lightness 13
  colors[ImGuiCol_ButtonActive]           = get_float_colour(13);

  // lightness 12
  colors[ImGuiCol_ScrollbarGrabActive]    = get_float_colour(12);

  // lightness 11
  colors[ImGuiCol_Border]                 = get_float_colour(11, 0.50f);
  colors[ImGuiCol_Separator]              = get_float_colour(11, 0.50f);

  // lightness 10
  colors[ImGuiCol_ScrollbarGrabHovered]   = get_float_colour(10);
  colors[ImGuiCol_SeparatorHovered]       = get_float_colour(10, 0.78f);
  colors[ImGuiCol_SeparatorActive]        = get_float_colour(10);
  colors[ImGuiCol_TabActive]              = get_float_colour(10);

  // lightness 9
  colors[ImGuiCol_Tab]                    = get_float_colour(9, 0.86f);

  // lightness 8
  colors[ImGuiCol_TableBorderStrong]      = get_float_colour(8);
  colors[ImGuiCol_FrameBg]                = get_float_colour(8, 0.54f);
  colors[ImGuiCol_TitleBgActive]          = get_float_colour(8);

  // lightness 7
  colors[ImGuiCol_ScrollbarGrab]          = get_float_colour(7);
  colors[ImGuiCol_TabUnfocusedActive]     = get_float_colour(7);

  // lightness 6
  colors[ImGuiCol_TableBorderLight]       = get_float_colour(6);

  // lightness 5
  colors[ImGuiCol_DockingEmptyBg]         = get_float_colour(5);
  colors[ImGuiCol_TableHeaderBg]          = get_float_colour(5);

  // lightness 4
  // no overrides

  // lightness 3
  colors[ImGuiCol_MenuBarBg]              = get_float_colour(3);
  colors[ImGuiCol_TabUnfocused]           = get_float_colour(3);

  // lightness 2
  colors[ImGuiCol_PopupBg]                = get_float_colour(2, 0.94f);
  colors[ImGuiCol_WindowBg]               = get_float_colour(2, 0.90f);

  // lightness 1
  colors[ImGuiCol_TitleBg]                = get_float_colour(1);
  colors[ImGuiCol_ScrollbarBg]            = get_float_colour(1, 0.53f);

  // lightness 0
  // no overrides

  ImGui_ImplGlfw_InitForOpenGL(window.glfw_window, true);                       // set up imgui backends
  ImGui_ImplOpenGL3_Init("#version 300 es");                                    // version 300 es for OpenGL ES 3.0 applies to WebGL 2.0
}

void shutdown() {
  /// Clear up ImGUI
  ImGui_ImplOpenGL3_Shutdown();                                                 // shutdown in reverse order of initialisation
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();
}

void restart(render::window &window) {
  /// Cleanly bounce ImGUI, resetting all state
  shutdown();
  init(window);
}

void frame_begin() {
  /// Helper to tidy up repetitive gui setup at the start of a frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void frame_end(render::window &window) {
  /// Helper to tidy up repetitive gui setup at the end of a frame
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window.glfw_window);
}


}
