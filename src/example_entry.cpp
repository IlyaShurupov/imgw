
#include "IMGUICLASS.H"

void ImGui::ShowDemoWindow(bool* p_open);

class Example : public ImGui::CompleteApp {
	void MainDrawTick() {
	bool open = true;
		ImGui::ShowDemoWindow(&open);
	}
};

int main() {
	Example app;
	app.Run();
}