
#include "DebugGui.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

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
	} else {
		a = pa;
	}
	if (active) return ImVec4((base_col + ac.r) / 255.f, (base_col + ac.g) / 255.f, (base_col + ac.b) / 255.f, a);
	return ImVec4((base_col + ic.r) / 255.f, (base_col + ic.g) / 255.f, (base_col + ic.b) / 255.f, a);
}

void DebugGui::StyleEditor() {
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

void DebugGui::apply_style() {

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