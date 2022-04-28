#pragma once

#include "imgui.h"

class DebugGui {

	public:
	DebugGui(struct GLFWwindow* window);

	void frame_start();

	void WindowMain(const char* name);
	bool WindowEditor(const char* name);

	void StyleEditor();

	bool SubMenuBegin(const char* desc, int level);
	void SubMenuEnd(int level);

	void Notify(const char* desc, int codec = 0);
	void ToolTip(const char* desc);

	void frame_end();

	~DebugGui();

	private:

	void apply_style();
};
