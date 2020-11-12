#pragma once

#include <utility>

namespace SG
{

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);

		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};

}