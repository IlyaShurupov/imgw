#pragma once

#include "ImGuiClass.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#pragma warning(disable:4174)
#include "imgui_notify.h"
#pragma warning(default:4174)
#include "tahoma.h" // <-- Required font!

#include "implot.h"
#include "imgui_internal.h"

#include "typelist.h"
#include "strings.h"
#include "stack.h"
#include "map.h"
#include "npple.h"

using namespace ImGui;

int ImGui::frame_window = (ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration);

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

void DefaultWraper::init(tp::ogl::window* window) {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
	// Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
	// Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window->winp, true);
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

	winp = window;
}

DefaultWraper::DefaultWraper() {}

void DefaultWraper::frame_start() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	draw_list = ImGui::GetForegroundDrawList();
}

void ImGui::WindowMain(const char* name) {
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

bool ImGui::WindowEditor(const char* name) {
	bool open;
	return ImGui::Begin(name, &open, ImGuiWindowFlags_NoCollapse);
}

bool ImGui::SubMenuBegin(const char* desc, int level) {
	if (level == 1) {
		ImGui::Separator();
	}
	if (ImGui::TreeNode(desc)) {
		GImGui->Style.IndentSpacing = 6;
		//ImGui::Unindent();
		return true;
	}
	return false;
}

void ImGui::SubMenuEnd(int level) {
	ImGui::TreePop();
	//ImGui::Indent();
}

void ImGui::Notify(const char* desc, int codec) {
	ImGui::InsertNotification({codec, 3000, desc});
}

void ImGui::ToolTip(const char* desc) {
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

void DefaultWraper::frame_end() {
	render_notify();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DefaultWraper::DrawRectF(tp::rectf rect, tp::rgba col, tp::halnf roundness) {
	rect.invy(winp->size.y);
	draw_list->AddRectFilled(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.z, rect.y + rect.w),
		ImColor(col.r, col.g, col.b, col.a), roundness);
}

void DefaultWraper::DrawCircle(tp::vec2f center, tp::halnf radius, tp::rgba col, tp::halnf thickness) {
	center.y = winp->size.y - center.y;
	draw_list->AddCircle(ImVec2(center.x, center.y), radius, ImColor(col.r, col.g, col.b, col.a), 32, thickness);
}

void DefaultWraper::DrawCircleF(tp::vec2f center, tp::halnf radius, tp::rgba col) {
	center.y = winp->size.y - center.y;
	draw_list->AddCircleFilled(ImVec2(center.x, center.y), radius, ImColor(col.r, col.g, col.b, col.a), 32);
}

void DefaultWraper::DrawLine(tp::vec2f p1, tp::vec2f p2, tp::rgba col, tp::halnf thickness) {
	p1.y = winp->size.y - p1.y;
	p2.y = winp->size.y - p2.y;

	draw_list->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImColor(col.r, col.g, col.b, col.a), thickness);
}

void ImGui::DefaultWraper::DrawTextR(tp::rectf rec, const char* text, tp::rgba col) {
	rec.invy(winp->size.y);
	draw_list->AddText(ImVec2(rec.pos.x, rec.pos.y), ImColor(col.r, col.g, col.b, col.a), text);
}

void ImGui::DefaultWraper::Texture(tp::rectf rec, const char* TextureId, GLuint buff) {
	GLuint tex = tp::get_tex(TextureId);
	if (tex) {
		winp->set_viewport(rec);
		tp::draw_texture(buff, tex);
	}
}

void ImGui::DefaultWraper::DrawTrigF(tp::vec2f p1, tp::vec2f p2, tp::vec2f p3, tp::rgba col) {
	p1.y = winp->size.y - p1.y;
	p2.y = winp->size.y - p2.y;
	p3.y = winp->size.y - p3.y;

	draw_list->AddTriangleFilled(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImVec2(p3.x, p3.y), ImColor(col.r, col.g, col.b, col.a));
}

DefaultWraper::~DefaultWraper() {
	ImPlot::DestroyContext();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

}


CompleteApp::CompleteApp() {
	init(&window);

	window.col_clear = tp::rgba(0.22f, 0.22f, 0.24f, 0.96f);
}

CompleteApp::CompleteApp(const tp::vec2f& rect, tp::alni window_params, const char* style) : window(rect, window_params) {
	init(&window);
	window.col_clear = tp::rgba(0.22f, 0.22f, 0.24f, 0.96f);

	if (style) StyleEditorLoad(style);
	apply_style();
}

void CompleteApp::Run() {

	while (!window.CloseSignal()) {

		window.update_event_queue(whait_for_event && (!need_update));
		window.begin_draw(need_update);

		if (window.new_frame) {
			if (clear_frames) {
				window.clear();
			}
			window.reset_viewport();
			frame_start();
		}

		// Main Proc
		MainProcTick();

		if (window.draw_event) {

			if (main_window) {
				WindowMain("MainWindow");
			}

			if (debug_info) {
				draw_debug_info();
			}

			// Main Draw
			MainDrawTick();

			gui_is_active = ImGui::GetIO().WantCaptureMouse;

			frame_end();
		}

		if (window.SpecialKey()) {
			debug_info = !debug_info;
		}


		static bool prev_state = 0;
		bool state = glfwGetMouseButton(window.winp, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
		if (state == prev_state) {
			need_update = false;
		} else {
			prev_state = state;
			need_update = true;
		}

		window.end_draw();

		if (window.draw_event) {
			window_fps.update(false);
		}

		fps.update(false);
	}
}

void CompleteApp::draw_debug_info() {
	WindowEditor("Debug Info");

	Text("InputProc FPS: %i", fps.fps);
	Text("WindowDraw Fps: %i", window_fps.fps);

	End();
}

ImGui::ImGuiPopupData ImGui::ButtonHoverPopupBegin(const char* id, tp::vec2f butsize, tp::vec2f popupsize) {
	ImVec2 curs = ImGui::GetCursorScreenPos();
	tp::vec2f popup_pos = tp::vec2f(curs.x + butsize.x / 2.f - popupsize.x / 2.f, (curs.y + butsize.y + 7));
	ImGui::Button(id, ImVec2(butsize.x, butsize.y)); ImGui::SameLine();
	return HoverPopupBegin(id, popupsize, popup_pos);
}

ImGui::ImGuiPopupData ImGui::HoverPopupBegin(const char* str_id, tp::vec2f size, tp::vec2f pos_p, ImGuiPopupFlags popup_flags) {
	ImGui::ImGuiPopupData out;
	out.ishovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

	if (out.ishovered) {
		ImVec2 pos;
		
		if (pos_p == tp::vec2f(-1)) {
			pos = GImGui->CurrentWindow->DC.CursorPos;
		} else {
			pos.x = pos_p.x;
			pos.y = pos_p.y;
		}

		ImGui::SetNextWindowPos(pos);
		out.p1 = {pos.x, pos.y};

		ImGui::OpenPopup(str_id);

		out.p2 = out.p1;
		out.p2.x += ImGui::GetWindowWidth();
	}

	ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

	if (BeginPopup(str_id, ImGuiWindowFlags_NoMove)) {
		out.opened = true;

		auto pos = GetWindowPos();
		out.p1 = {pos.x, pos.y};
		out.p2 = out.p1;
		out.p2.x += ImGui::GetWindowWidth();

	}
	return out;
}

void ImGui::HoverPopupEnd(ImGui::ImGuiPopupData& in) {

	if (!in.opened) {
		return;
	}

	in.ishovered |= IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_ChildWindows);

	tp::vec2f mousepos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
	tp::halnf tollerance = 10;
	tp::rectf tollerace_rect = tp::rectf(tp::vec2f(in.p1.x, in.p1.y - tollerance), tp::vec2f(in.p2.x - in.p1.x, tollerance * 2.f));
	bool is_tollerance = tollerace_rect.inside(mousepos);

	if (!(in.ishovered || is_tollerance)) {
		CloseCurrentPopup();
	}

	EndPopup();
}