#pragma once

#include "imgui.h"

class imGuiDefaultApp {

  bool initialized = true;
  time_t fdur = time_t(1000 / 60.f);
  struct GLFWwindow* window;

public:
  imGuiDefaultApp();
  virtual bool mainloop_tick();
  void mainloop();

  ~imGuiDefaultApp();
};

const char* get_font_path();

void MainWindow(const char* name);
void ToolTip(const char* desc);
void apply_style();

bool SubMenuBegin(const char* desc, int level);
bool SubMenuItemBegin(const char* desc);
void SubMenuItemEnd();
void SubMenuEnd(int level);
ImVec2 AlightRight();
bool EditorWindow(const char* name);

void init_notify();
void example_nofify();
void render_notify();
void Notify(const char* desc, int codec = 0);