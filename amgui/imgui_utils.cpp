#pragma once

#include "DebugGui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_notify.h"
#include "tahoma.h" // <-- Required font!

#include "implot.h"

const char* get_font_path() {
	static bool path_calculated = false;

	static char path[200];

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
			for (int i = 0; i < 8; i++, start++) {
				path[start] = font_name[i];
			}
			path[start] = '\0';
			break;
		}
	}

	path_calculated = true;
	return path;
}


DebugGui::DebugGui(struct GLFWwindow* window) {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
	// Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
	// Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	// ImPlot::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImFont* font1 = io.Fonts->AddFontFromFileTTF(get_font_path(), 18.0f);

	apply_style();

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF((void*) tahoma, sizeof(tahoma), 17.f, &font_cfg);

	// Initialize notify
	ImGui::MergeIconsWithLatestFont(16.f, false);

	//io->Fonts->AddFontFromMemoryTTF((void*)another_font, sizeof(another_font), 17.f, &font_cfg);
	//ImGui::MergeIconsWithLatestFont(16.f, false);

	ImPlot::CreateContext();
}

void DebugGui::frame_start() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DebugGui::WindowMain(const char* name) {
	bool p_open = 1;

	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) {
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	} else {
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

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

bool DebugGui::WindowEditor(const char* name) {
	bool open;
	return ImGui::Begin(name, &open, ImGuiWindowFlags_NoCollapse);
}

bool DebugGui::SubMenuBegin(const char* desc, int level) {
	if (level == 1) {
		ImGui::Separator();
	}
	if (ImGui::TreeNode(desc)) {
		ImGui::Unindent();
		return true;
	}
	return false;
}

void DebugGui::SubMenuEnd(int level) {
	ImGui::TreePop();
	ImGui::Indent();
}

void DebugGui::Notify(const char* desc, int codec) {
	ImGui::InsertNotification({codec, 3000, desc});
}

void DebugGui::ToolTip(const char* desc) {
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

void render_notify() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
	ImGui::RenderNotifications(); // <-- Here we render all notifications
	ImGui::PopStyleVar(1); // Don't forget to Pop()
	ImGui::PopStyleColor(1);
	//ImGui::notifications.~vector();
}

void DebugGui::frame_end() {
	render_notify();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

DebugGui::~DebugGui() {
	ImPlot::DestroyContext();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

}
