
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> 

#include <iostream>

#include "implot.h"
#include "imgui_utils.h"

#include <chrono>
#include <thread>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#define GETTIMEMSC() (time_t)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch()).count())
#define THREAD_SLEEP(time_ms) std::this_thread::sleep_for(std::chrono::milliseconds(time_ms))

void imGuiWindowInitialize();
void imGuiWindowDraw();
void imGuiWindowFinalize();

int WinMain(int, char**);

int main() {
  return WinMain(NULL, NULL);
}

#define MAX_FONT_PATH 200
#define FONT_NAME_LEN 8
static bool path_calculated = false;

const char* get_font_path() {
  
  static char path[MAX_FONT_PATH];
  
  const char* this_path = __FILE__;
  const char* font_name = "font.ttf";

  if (path_calculated) {
    return path;
  }

  int len = 0;
  for (const char* i = this_path; *i != '\0'; i++, len++) {
    path[len] = *i;
  }
  path[len] = '\0';

  for (int start = len; start > 0; start--) {
    if (path[start] == '\\' || path[start] == '/') {
      start++;
      for (int i = 0; i < FONT_NAME_LEN; i++, start++) {
        path[start] = font_name[i];
      }
      path[start] = '\0';
      break;
    }
  }

  path_calculated = true;
  return path;
}

int WinMain(int, char**) {

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return 1;
  }

  const char* glsl_version = "#version 130";
  //glfwWindowHint(GLFW_DECORATED, false);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

  GLFWwindow* window = glfwCreateWindow(1280, 720, " ", NULL, NULL);
  if (window == NULL) {
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImPlot::CreateContext();
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImFont* font1 = io.Fonts->AddFontFromFileTTF(get_font_path(), 18.0f);

  init_notify();

  time_t fdur = 1000 / 60.f;

  imGuiWindowInitialize();

  while (!glfwWindowShouldClose(window)) {
    
    time_t fstart = GETTIMEMSC();

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    MainWindow("as");

    imGuiWindowDraw();
    
    render_notify();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);

    time_t fend = GETTIMEMSC();
    time_t rem = fdur - (fend - fstart);
    if (rem > 0) {
      THREAD_SLEEP(rem);
    }
  }


  // Cleanup
  ImPlot::DestroyContext();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  imGuiWindowFinalize();

  return 0;
}
