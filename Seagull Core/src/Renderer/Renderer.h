#pragma once

namespace SG
{

	class Renderer
	{
	public:
		Renderer() = default;
		virtual ~Renderer() = default;

		static void Init();
	private:
	};

}