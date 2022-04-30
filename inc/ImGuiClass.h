#pragma once

#include "imgui.h"

#include "gl.h"
#include "glutils.h"
#include "window.h"

namespace ImGui {

	void StyleEditor();

	bool WindowEditor(const char* name);
	bool SubMenuBegin(const char* desc, int level);
	void SubMenuEnd(int level);
	void WindowMain(const char* name);

	void Notify(const char* desc, int codec = 0);
	void ToolTip(const char* desc);

	void apply_style();

	class DefaultWraper {

		ImDrawList* draw_list = NULL;
		ogl::window* winp;

		public:

		DefaultWraper();
		void init(ogl::window* window);

		void frame_start();

		void frame_end();

		virtual ~DefaultWraper();

		void DrawRectF(rectf rect, rgba col, halnf roundness);
		void DrawCircle(vec2f center, halnf radius, rgba col, halnf thickness = 2.f);
		void DrawCircleF(vec2f center, halnf radius, rgba col);
		void DrawTrigF(vec2f p1, vec2f p2, vec2f p3, rgba col);
		void DrawLine(vec2f p1, vec2f p2, rgba col, halnf thickness);
		void Texture(rectf rec, const char* TextureId, GLuint buff = 0);

		private:
	};

	class CompleteApp : public DefaultWraper {

		fpscount fps;
		fpscount window_fps;

		public:

		ogl::opengl gl;
		ogl::window window;

		bool main_window = true;
		bool debug_info = false;
		bool whait_for_event = true;
		bool need_update = false;
		bool gui_is_active = false;
		bool clear_frames = true;
		halni max_proc_fps = 100;

		CompleteApp();
		CompleteApp(const vec2f& rect, alni window_params = 0);

		virtual void MainProcTick() {}
		virtual void MainDrawTick() { Text("Hello"); }

		void Run();
		void draw_debug_info();
		virtual ~CompleteApp() {}
	};
};