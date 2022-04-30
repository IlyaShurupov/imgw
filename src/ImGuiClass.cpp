#pragma once

#include "ImGuiClass.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


#pragma warning(disable:4174)
#include "imgui_notify.h"
#pragma warning(default:4174)
#include "tahoma.h" // <-- Required font!

#include "implot.h"

using namespace ImGui;

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

void DefaultWraper::init(ogl::window* window) {
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

void DefaultWraper::WindowMain(const char* name) {
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

bool DefaultWraper::WindowEditor(const char* name) {
	bool open;
	return ImGui::Begin(name, &open, ImGuiWindowFlags_NoCollapse);
}

bool DefaultWraper::SubMenuBegin(const char* desc, int level) {
	if (level == 1) {
		ImGui::Separator();
	}
	if (ImGui::TreeNode(desc)) {
		ImGui::Unindent();
		return true;
	}
	return false;
}

void DefaultWraper::SubMenuEnd(int level) {
	ImGui::TreePop();
	ImGui::Indent();
}

void DefaultWraper::Notify(const char* desc, int codec) {
	ImGui::InsertNotification({codec, 3000, desc});
}

void DefaultWraper::ToolTip(const char* desc) {
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

void DefaultWraper::DrawRectF(rectf rect, rgba col, halnf roundness) {
	rect.invy(winp->size.y);
	draw_list->AddRectFilled(ImVec2(rect.x, rect.y), ImVec2(rect.x + rect.z, rect.y + rect.w),
		ImColor(col.r, col.g, col.b, col.a), roundness);
}

void DefaultWraper::DrawCircle(vec2f center, halnf radius, rgba col, halnf thickness) {
	center.y = winp->size.y - center.y;
	draw_list->AddCircle(ImVec2(center.x, center.y), radius, ImColor(col.r, col.g, col.b, col.a), 32, thickness);
}

void DefaultWraper::DrawCircleF(vec2f center, halnf radius, rgba col) {
	center.y = winp->size.y - center.y;
	draw_list->AddCircleFilled(ImVec2(center.x, center.y), radius, ImColor(col.r, col.g, col.b, col.a), 32);
}

void DefaultWraper::DrawLine(vec2f p1, vec2f p2, rgba col, halnf thickness) {
	p1.y = winp->size.y - p1.y;
	p2.y = winp->size.y - p2.y;

	draw_list->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImColor(col.r, col.g, col.b, col.a), thickness);
}

void ImGui::DefaultWraper::Texture(rectf rec, const char* TextureId, GLuint buff) {
	GLuint tex = get_tex(TextureId);
	if (tex) {
		winp->set_viewport(rec);
		draw_texture(buff, tex);
	}
}

void ImGui::DefaultWraper::DrawTrigF(vec2f p1, vec2f p2, vec2f p3, rgba col) {
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

// Examples
static void CustomRend() {
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	draw_list->AddRectFilled(ImVec2(2, 2), ImVec2(32, 32), ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f)), 10.0f);

	return;

	static float sz = 36.0f;
	static float thickness = 4.0f;
	static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);

	{
		const ImVec2 p = ImVec2(0, 0);
		const ImU32 col32 = ImColor(col);
		float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;

		//float th = thickness;
		//draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz), col32, th);
		//x = p.x + 4;
		//y += sz + spacing;

		draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 32); x += sz + spacing;      // Circle
		//draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 32); x += sz + spacing;      // Circle
		draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f); x += sz + spacing;

		//draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));

	}
}


CompleteApp::CompleteApp() {
	init(&window);

	window.col_clear = rgba(0.22f, 0.22f, 0.24f, 0.96f);
}

CompleteApp::CompleteApp(const vec2f& rect, alni window_params) : window(rect, window_params) {
	init(&window);
	window.col_clear = rgba(0.22f, 0.22f, 0.24f, 0.96f);
}

void CompleteApp::Run() {

	while (!window.CloseSignal()) {

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

		window.end_draw(whait_for_event && (!need_update));

		if (need_update) {
			need_update = false;
		}

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