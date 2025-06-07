#pragma once
#include <d3d9.h>
#include "../imgui/imgui.h"
#include "../modules/spoofer/spoofdid.h"
#include "../Utils/MemoWriter.h"

static char buffer[128] = ""; // Buffer for text input
static MemoWriter memoWriter("Minecraft.Windows.exe"); // Replace with your target process


namespace clicker {

	inline bool LeftClicker;
	inline bool RightClicker;
	inline bool OnlyMinecraft;
	inline int minCPS = 1;
	inline int maxCPS = 2;
	inline int minRCPS = 1;
	inline int maxRCPS = 2;
	
}

namespace utilities {

	inline bool nofall;
	inline bool velocity;
	inline bool velocityy;
	inline bool velocityz;
	inline bool blink;
	inline bool glide;

}

namespace reach {

	inline bool C_reach;
	inline float reachValue = 3;

}

namespace lag {

	inline bool lagrange;
	inline int cooldown;
	inline int everyms;

}

namespace ui {
	void init(LPDIRECT3DDEVICE9);
	void render();
}

namespace ui {
	inline LPDIRECT3DDEVICE9 dev;
	inline const char* window_title = "Uranium | 1.0";
}

namespace ui {
	inline ImVec2 screen_res{ 000, 000 };
	inline ImVec2 window_pos{ 0, 0 };
	inline ImVec2 window_size{ 400, 300 };
	inline DWORD  window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
}