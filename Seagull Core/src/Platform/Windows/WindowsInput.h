#pragma once

#include "Input/Input.h"


namespace SG
{

	class WindowsInput : public Input 
	{
	public:
		virtual bool IsKeyPressedImpl(int keycode) const override;
		virtual bool IsMouseButtonPressedImpl(int button) const override;

		virtual std::pair<float, float> GetMousePosImpl() const override;
		virtual float GetMouseXImpl() const override;
		virtual float GetMouseYImpl() const override;
	};

}