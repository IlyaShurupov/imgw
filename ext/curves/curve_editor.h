
#pragma once

#include <imgui.h>
#include <imgui_internal.h>

namespace ImGui {
	template<int steps>
	void bezier_table(ImVec2 P[4], ImVec2 results[steps + 1]) {
		static float C[(steps + 1) * 4], * K = 0;
		if (!K) {
			K = C;
			for (unsigned step = 0; step <= steps; ++step) {
				float t = (float) step / (float) steps;
				C[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t);   // * P0
				C[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t; // * P1
				C[step * 4 + 2] = 3 * (1 - t) * t * t;     // * P2
				C[step * 4 + 3] = t * t * t;               // * P3
			}
		}
		for (unsigned step = 0; step <= steps; ++step) {
			ImVec2 point = {
					K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x + K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
					K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y + K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y
			};
			results[step] = point;
		}
	}

	float BezierValue(float dt01, float P[4]);

	int Bezier(const char* label, float P[5]);
};