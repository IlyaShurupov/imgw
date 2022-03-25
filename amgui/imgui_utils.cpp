#pragma once

#include "imgui_utils.h"

#include "imgui_notify.h"
#include "tahoma.h" // <-- Required font!

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

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


struct inactive_col {
  int r = 0;
  int g = 0;
  int b = 10;
};

struct active_col {
  int r = 40;
  int g = 30;
  int b = 10;
};

active_col ac;
inactive_col ic;

int gray = 120;
int gray_step = 10;

int black = 70;
int black_step = 10;

int white = 255;
int white_step = 10;

int butt_width = 60;
int butt_height = 30;

char style_save_path[100] = "style";
bool first_init = true;

void StyleEditorSave(const char* name) {
  ofstream fout;
  fout.open(name);
  
  if (!fout) {
    return;
  }

  fout << black << endl;
  fout << black_step << endl;
  fout << gray << endl;
  fout << gray_step << endl;
  fout << white << endl;
  fout << white_step << endl;
  
  fout.close();
}

void StyleEditorLoad(const char* name) {
  ifstream fin;
  string line;
  fin.open(name);

  if (!fin) {
    return;
  }

  fin >> black;
  fin >> black_step;
  fin >> gray;
  fin >> gray_step;
  fin >> white;
  fin >> white_step;

  fin.close();
}

void StyleEditor() {
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
  ImGui::SliderInt("Black", &black, 0, 255); ImGui::SameLine(); ImGui::SliderInt("BStep", &black_step, 0, 100);
  ImGui::SliderInt("Gray", &gray, 0, 255); ImGui::SameLine(); ImGui::SliderInt("GStep", &gray_step, 0, 100);
  ImGui::SliderInt("White", &white, 0, 255); ImGui::SameLine(); ImGui::SliderInt("WStep", &white_step, 0, 100);

  ImGui::PopItemWidth();

  ImGui::InputText("Style Name", style_save_path, 100);
  if (ImGui::Button("Save")) {
    StyleEditorSave(style_save_path);
  } ImGui::SameLine();
  if (ImGui::Button("Load")) {
    StyleEditorLoad(style_save_path);
  }
}

ImVec4 gui_col(int range, int child_range, bool active = false, bool child = false, float pa = -1.f) {

  int base_col = 0;
  switch (range) {
  case 1:
    base_col = white;
    base_col -= child_range * white_step;
    break;
  case 2:
    base_col = gray;
    base_col -= child_range * gray_step;
    break;
  case 3:
    base_col = black;
    base_col -= child_range * black_step;
    break;
  }
  float a;
  if (pa == -1) {
    a = child * 0.5f + !child * 1;
  }
  else {
    a = pa;
  }
  if (active) return ImVec4((base_col + ac.r) / 255.f, (base_col + ac.g) / 255.f, (base_col + ac.b) / 255.f, a);
  return ImVec4((base_col + ic.r) / 255.f, (base_col + ic.g) / 255.f, (base_col + ic.b) / 255.f, a);
}


void apply_style() {

  if (first_init) {
    StyleEditorLoad(style_save_path);
    first_init = false;
  }

  ImGuiStyle* style = &ImGui::GetStyle();

  style->WindowPadding = ImVec2(15, 15);
  style->WindowRounding = 5.0f;
  style->FramePadding = ImVec2(5, 5);
  
  style->ChildRounding = 10.0f;
  style->PopupRounding = 10.0f;
  style->FrameRounding = 10.0f;
  style->GrabRounding = 10.0f;

  style->FrameBorderSize = 1.0f;
  style->ItemSpacing = ImVec2(12, 8);
  style->ItemInnerSpacing = ImVec2(8, 6);
  style->IndentSpacing = 25.0f;
  style->ScrollbarSize = 15.0f;
  style->ScrollbarRounding = 9.0f;
  style->GrabMinSize = 5.0f;
  style->WindowBorderSize = 3.0f;
  style->ChildBorderSize = 3.0f;

  //style->Colors[ImGuiCol_BorderShadow] = gui_col(3, 7);

  style->Colors[ImGuiCol_Border] = gui_col(2, 1);
  style->Colors[ImGuiCol_Separator] = gui_col(2, 2);

  style->Colors[ImGuiCol_Text] = gui_col(1, 5);
  style->Colors[ImGuiCol_TextDisabled] = gui_col(1, 9);
  style->Colors[ImGuiCol_WindowBg] = gui_col(3, 5);
  style->Colors[ImGuiCol_ButtonActive] = gui_col(2, 0);
  style->Colors[ImGuiCol_Button] = gui_col(3, 5);
  style->Colors[ImGuiCol_ChildBg] = gui_col(3, 3);
  style->Colors[ImGuiCol_PopupBg] = gui_col(3, 3);
  style->Colors[ImGuiCol_FrameBg] = gui_col(3, 4);
  style->Colors[ImGuiCol_FrameBgHovered] = gui_col(3, 3);
  style->Colors[ImGuiCol_FrameBgActive] = gui_col(3, 3);
  style->Colors[ImGuiCol_TitleBg] = gui_col(3, 3);
  style->Colors[ImGuiCol_TitleBgCollapsed] = gui_col(3, 3);
  style->Colors[ImGuiCol_TitleBgActive] = gui_col(3, 3);
  style->Colors[ImGuiCol_MenuBarBg] = gui_col(3, 3);
  style->Colors[ImGuiCol_ScrollbarBg] = gui_col(3, 3);
  style->Colors[ImGuiCol_ScrollbarGrab] = gui_col(3, 0);
  style->Colors[ImGuiCol_ScrollbarGrabHovered] = gui_col(2, 1);
  style->Colors[ImGuiCol_ScrollbarGrabActive] = gui_col(2, 0);
  //style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
  style->Colors[ImGuiCol_CheckMark] = gui_col(2, 0);
  style->Colors[ImGuiCol_SliderGrab] = gui_col(1, 3, 1);
  style->Colors[ImGuiCol_SliderGrabActive] = gui_col(1, 2, 1);
  style->Colors[ImGuiCol_Button] = gui_col(3, 2);
  style->Colors[ImGuiCol_ButtonHovered] = gui_col(2, 2);
  style->Colors[ImGuiCol_ButtonActive] = gui_col(2, 0);
  style->Colors[ImGuiCol_Header] = gui_col(3, 3);
  style->Colors[ImGuiCol_HeaderHovered] = gui_col(3, 2);
  style->Colors[ImGuiCol_HeaderActive] = gui_col(3, 1);
  //style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  //style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
  //style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
  style->Colors[ImGuiCol_ResizeGrip] = gui_col(3, 3);
  style->Colors[ImGuiCol_ResizeGripHovered] = gui_col(3, 3);
  style->Colors[ImGuiCol_ResizeGripActive] = gui_col(3, 3);
  //style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
  //style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
  //style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
  style->Colors[ImGuiCol_PlotLines] = gui_col(3, 3);
  style->Colors[ImGuiCol_PlotLinesHovered] = gui_col(3, 3);
  style->Colors[ImGuiCol_PlotHistogram] = gui_col(3, 3);
  style->Colors[ImGuiCol_PlotHistogramHovered] = gui_col(3, 3);
  style->Colors[ImGuiCol_TextSelectedBg] = gui_col(2, 4);
  //style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

  //style->Colors[ImGuiCol_Hover] = gui_col(3, 3);

  style->Colors[ImGuiCol_Tab] = gui_col(3, 3);
  style->Colors[ImGuiCol_FrameBg] = gui_col(3, 3);
  style->Colors[ImGuiCol_MenuBarBg] = gui_col(3, 4);
  style->Colors[ImGuiCol_Header] = gui_col(3, 3);
  style->Colors[ImGuiCol_HeaderActive] = gui_col(3, 3);
  style->Colors[ImGuiCol_HeaderHovered] = gui_col(3, 3);
  style->Colors[ImGuiCol_TabUnfocused] = gui_col(3, 3);
  style->Colors[ImGuiCol_DockingEmptyBg] = gui_col(3, 6);
  style->Colors[ImGuiCol_DockingPreview] = gui_col(2, 3);
  style->Colors[ImGuiCol_TabHovered] = gui_col(3, 4);
  style->Colors[ImGuiCol_TabUnfocusedActive] = gui_col(3, 5);
  style->Colors[ImGuiCol_TabActive] = gui_col(3, 5);



  style->WindowMenuButtonPosition = ImGuiDir_Right;
}

void MainWindow(const char* name) {
  bool p_open = 1;
  // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
  // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
  // In this specific demo, we are not using DockSpaceOverViewport() because:
  // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
  // - we allow the host window to have padding (when opt_padding == true)
  // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
  // TL;DR; this demo is more complicated than what you would normally use.
  // If we removed all the options we are showcasing, this demo would become:
  //     void ShowExampleAppDockSpace()
  //     {
  //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
  //     }

  static bool opt_fullscreen = true;
  static bool opt_padding = false;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
  // because it would be confusing to have two docking targets within each others.
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen)
  {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }
  else
  {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
  // and handle the pass-thru hole, so we ask Begin() to not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
  // all active windows docked into it will lose their parent and become undocked.
  // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
  // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
  if (!opt_padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin(name, &p_open, window_flags);
  if (!opt_padding)
    ImGui::PopStyleVar();

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  // Submit the DockSpace
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("No Resize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
      if (ImGui::MenuItem("Auto Hide TabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
      ImGui::Separator();
      StyleEditor();
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  apply_style();

  ImGui::End();
}


void ToolTip(const char* desc) {
  ImGui::SameLine();
  ImGui::TextDisabled("*");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

bool SubMenuBegin(const char* desc, int level) {
  if (level == 1) {
    ImGui::Separator();
  }
  if (ImGui::TreeNode(desc)) {
    ImGui::Unindent();
    return true;
  }
  return false;
}

bool SubMenuItemBegin(const char* desc) {
  return false;
}

void SubMenuItemEnd() {
}

void SubMenuEnd(int level) {
  ImGui::TreePop();
  ImGui::Indent();
}

ImVec2 AlightRight() {
  ImGui::Spacing();
  ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - butt_width);
  return ImVec2((float)butt_width, (float)butt_height);
}

bool EditorWindow(const char* name) {
  bool open;
  return ImGui::Begin(name, &open, ImGuiWindowFlags_NoCollapse);
}

void init_notify() {
  ImGuiIO* io = &ImGui::GetIO();

  ImFontConfig font_cfg;
  font_cfg.FontDataOwnedByAtlas = false;
  io->Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 17.f, &font_cfg);

  // Initialize notify
  ImGui::MergeIconsWithLatestFont(16.f, false);
   
  //io->Fonts->AddFontFromMemoryTTF((void*)another_font, sizeof(another_font), 17.f, &font_cfg);
  //ImGui::MergeIconsWithLatestFont(16.f, false);

}

/*
  ImGuiToastType_None,
  ImGuiToastType_Success,
  ImGuiToastType_Warning,
  ImGuiToastType_Error,
  ImGuiToastType_Info,
  ImGuiToastType_COUNT
*/
void Notify(const char* desc, int codec) {
  ImGui::InsertNotification({ codec, 3000, desc });
}

void example_nofify() {
  {
    ImGui::SetNextWindowPos({ 50, 50 }, ImGuiCond_Once);
    ImGui::SetNextWindowSize({ 550, 550 }, ImGuiCond_Once);
    ImGui::Begin("Hello World!", NULL, NULL);

    if (ImGui::CollapsingHeader("Examples without title", ImGuiTreeNodeFlags_DefaultOpen))
    {
      if (ImGui::Button("Success"))
      {
        ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "Hello World! This is a success! %s", "We can also format here:)" });
      }

      ImGui::SameLine();
      if (ImGui::Button("Warning"))
      {
        ImGui::InsertNotification({ ImGuiToastType_Warning, 3000, "Hello World! This is a warning!" });
      }

      ImGui::SameLine();
      if (ImGui::Button("Error"))
      {
        ImGui::InsertNotification({ ImGuiToastType_Error, 3000, "Hello World! This is an error!" });
      }

      ImGui::SameLine();
      if (ImGui::Button("Info"))
      {
        ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Hello World! This is an info!" });
      }

      ImGui::SameLine();
      if (ImGui::Button("Info long"))
      {
        ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation" });
      }
    }

    if (ImGui::CollapsingHeader("Do it yourself", ImGuiTreeNodeFlags_DefaultOpen))
    {
      static char title[4096] = "A wonderful quote!";
      ImGui::InputTextMultiline("Title", title, sizeof(title));

      static char content[4096] = "Ours is a friendship forged once in this life, and again in the next. Goodbye, my brother. \n- Dembe Zuma";
      ImGui::InputTextMultiline("Content", content, sizeof(content));

      static int duration = 5000; // 5 seconds
      ImGui::InputInt("Duration (ms)", &duration, 100);
      if (duration < 0) duration = 0; // Shouldn't be negative

      const char* type_str[] = { "None", "Success", "Warning", "Error", "Info" };
      static ImGuiToastType type = ImGuiToastType_Success;
      IM_ASSERT(type < ImGuiToastType_COUNT);

      if (ImGui::BeginCombo("Type", type_str[type]))
      {
        for (auto n = 0; n < IM_ARRAYSIZE(type_str); n++)
        {
          const bool is_selected = (type == n);

          if (ImGui::Selectable(type_str[n], is_selected))
            type = n;

          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }

      static bool enable_title = true, enable_content = true;
      ImGui::Checkbox("Enable title", &enable_title);
      ImGui::SameLine();
      ImGui::Checkbox("Enable content", &enable_content);

      if (ImGui::Button("Show"))
      {
        ImGuiToast toast(type, duration);

        if (enable_title)
          toast.set_title(title);

        if (enable_content)
          toast.set_content(content);

        ImGui::InsertNotification(toast);
      }
    }

    ImGui::End();
  }
}

void render_notify() {
  // Render toasts on top of everything, at the end of your code!
  // You should push style vars here
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
  ImGui::RenderNotifications(); // <-- Here we render all notifications
  ImGui::PopStyleVar(1); // Don't forget to Pop()
  ImGui::PopStyleColor(1);
  
  //ImGui::notifications.~vector();
}

