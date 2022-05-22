#pragma once

#include "imgui.h"
#include "ImGuiTypesConversions.h"

#include "gl.h"
#include "glutils.h"
#include "window.h"

#include "../ext/curves/curve_editor.h"

namespace ImGui {

	extern int frame_window;

	void StyleEditor();

	bool WindowEditor(const char* name);
	bool SubMenuBegin(const char* desc, int level);
	void SubMenuEnd(int level);
	void WindowMain(const char* name);

	void Notify(const char* desc, int codec = 0);
	void ToolTip(const char* desc);

	void apply_style();
	void StyleEditorLoad(const char* name);

	struct ImGuiPopupData {
		bool opened = false;
		bool ishovered = false;
		tp::vec2f p1 = 0;
		tp::vec2f p2 = 0;
		operator bool() {
			return opened;
		}
	};

	ImGuiPopupData ButtonHoverPopupBegin(const char* id, tp::vec2f butsize = 200, tp::vec2f popupsize = 200);
	ImGuiPopupData HoverPopupBegin(const char* id, tp::vec2f size = 200, tp::vec2f pos = -1, ImGuiPopupFlags flags = 0);
	void HoverPopupEnd(ImGuiPopupData& in);

	class DefaultWraper {

		ImDrawList* draw_list = NULL;
		tp::ogl::window* winp;

		public:

		DefaultWraper();
		void init(tp::ogl::window* window);

		void frame_start();

		void frame_end();

		virtual ~DefaultWraper();

		void DrawRectF(tp::rectf rect, tp::rgba col, tp::halnf roundness);
		void DrawCircle(tp::vec2f center, tp::halnf radius, tp::rgba col, tp::halnf thickness = 2.f);
		void DrawCircleF(tp::vec2f center, tp::halnf radius, tp::rgba col);
		void DrawTrigF(tp::vec2f p1, tp::vec2f p2, tp::vec2f p3, tp::rgba col);
		void DrawLine(tp::vec2f p1, tp::vec2f p2, tp::rgba col, tp::halnf thickness);
		void DrawTextR(tp::rectf rec, const char* text, tp::rgba col);
		void Texture(tp::rectf rec, const char* TextureId, GLuint buff = 0);

		private:
	};

	class CompleteApp : public DefaultWraper {

		tp::fpscount fps;
		tp::fpscount window_fps;

		public:

		tp::ogl::opengl gl;
		tp::ogl::window window;

		bool main_window = true;
		bool debug_info = false;
		bool whait_for_event = true;
		bool need_update = false;
		bool gui_is_active = false;
		bool clear_frames = true;
		tp::halni max_proc_fps = 100;

		CompleteApp();
		CompleteApp(const tp::vec2f& rect, tp::alni window_params = 0, const char* style = 0);

		virtual void MainProcTick() {}
		virtual void MainDrawTick() { Text("Hello"); }

		void Run();
		void draw_debug_info();
		virtual ~CompleteApp() {}
	};
};