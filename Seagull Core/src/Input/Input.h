#pragma once

#include <utility>

namespace SG
{

	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);

		static std::pair<LONG, LONG> GetMousePos();
		static LONG GetMouseX();
		static LONG GetMouseY();
	};

}