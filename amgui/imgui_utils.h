#pragma once

#include "imgui.h"

void MainWindow(const char* name);
void style_edeting();
void ToolTip(const char* desc);

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