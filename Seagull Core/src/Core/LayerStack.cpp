#include "sgpch.h"
#include "LayerStack.h"

namespace SG
{

	LayerStack::~LayerStack()
	{
		for (auto layer : m_Layers)
			delete layer;
	}

	void LayerStack::PushLayer(Layer* layer) noexcept
	{
		m_Layers.emplace(m_Layers.begin() + m_InsertIndex, layer);
		++m_InsertIndex;
	}

	void LayerStack::PushOverlay(Layer* layer) noexcept
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer) noexcept
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (iter != m_Layers.end())
		{
			m_Layers.erase(iter);
			--m_InsertIndex;
		}
	}

	void LayerStack::PopOverlay(Layer* layer) noexcept
	{
		auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (iter != m_Layers.end())
		{
			m_Layers.erase(iter);
		}
	}

}