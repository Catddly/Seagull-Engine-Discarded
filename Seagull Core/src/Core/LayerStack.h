#pragma once

#include "Core/Layer.h"

#include <vector>

namespace SG
{

	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		std::vector<Layer*> GetLayers() const { return m_Layers; }

		void PushLayer(Layer* layer) noexcept;
		void PushOverlay(Layer* layer) noexcept;

		// It will destroy the layer instance at the same time
		void PopLayer(Layer* layer) noexcept;
		void PopOverlay(Layer* layer) noexcept;

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end()   { return m_Layers.end(); }
		std::vector<Layer*>::const_iterator cbegin() { return m_Layers.cbegin(); }
		std::vector<Layer*>::const_iterator cend()   { return m_Layers.cend(); }
	private:
		std::vector<Layer*> m_Layers;
		size_t m_InsertIndex = 0;
	};

}