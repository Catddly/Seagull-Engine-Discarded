#include "sgpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"

#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Platform/DirectX/DirectX12RendererAPI.h"

namespace SG
{

	static bool ImGui_ImplWin32_UpdateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return false;

		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			::SetCursor(NULL);
		}
		else
		{
			// Show OS mouse cursor
			LPTSTR win32_cursor = IDC_ARROW;
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
			case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
			case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
			case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
			case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
			case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
			case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
			case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
			case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
			}
			::SetCursor(::LoadCursor(NULL, win32_cursor));
		}
		return true;
	}

	void ImGuiLayer::OnAttach()
	{
		HWND hwnd = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		auto instance = (DirectX12RendererAPI*)(RenderCommand::GetRenderAPIInstance());

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(instance->m_D3dDevice.Get(), 2,
			DXGI_FORMAT_R8G8B8A8_UNORM, instance->m_SrvHeap->GetNative(),
			instance->m_SrvHeap->GetCPUStart(),
			instance->m_SrvHeap->GetGPUStart());
	}

	void ImGuiLayer::OnDettach()
	{
		auto instance = (DirectX12RendererAPI*)(RenderCommand::GetRenderAPIInstance());
		instance->m_RenderQueue->FlushCommandQueue();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate(DeltaTime dt)
	{

	}

	void ImGuiLayer::Begin()
	{
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImVec4 clear_color = ImVec4(1.0f, 0.85f, 0.80f, 1.00f);
		ImGuiIO& io = ImGui::GetIO();
		auto instance = static_cast<DirectX12RendererAPI*>(RenderCommand::GetRenderAPIInstance());

		// Rendering
		instance->m_RenderQueue->FlushCommandQueue();
		UINT backBufferIdx = instance->m_SwapChain->GetCurrBufferIndex();
		instance->m_RenderQueue->ResetAlloctor();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = instance->m_SwapChainBuffers[backBufferIdx].GetNativePtr();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		instance->m_CommandList->Reset(instance->m_RenderQueue->GetCommandAllocatorNative(), NULL);
		instance->m_CommandList->ResourceBarrier(1, &barrier);
		instance->m_CommandList->ClearRtv(instance->m_RtvHeap->GetView(backBufferIdx), (float*)&clear_color, 0, NULL);
		instance->m_CommandList->SetRenderTarget(1, &instance->m_RtvHeap->GetView(backBufferIdx), FALSE, NULL);
		instance->m_CommandList->SetDescriptorHeaps(1, instance->m_SrvHeap->GetNativeAddress());

		ImGui::Render();

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), instance->m_CommandList->GetCommandListNative());
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		instance->m_CommandList->ResourceBarrier(1, &barrier);
		instance->m_CommandList->Close();

		instance->m_RenderQueue->ExecuteCommandLists(instance->m_CommandList->GetCommandListNative());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault(NULL, (void*)instance->m_CommandList->GetCommandListNative());
		}

		instance->m_SwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync

		instance->m_RenderQueue->FlushCommandQueue();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return;

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnWindowResize));
		dispatcher.Dispatch<MouseButtonPressedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnMouseButtonReleased));
		dispatcher.Dispatch<MouseScrolledEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnMouseScrolled));
		dispatcher.Dispatch<KeyPressedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnKeyReleased));
		dispatcher.Dispatch<KeyTypedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnKeyTyped));
		dispatcher.Dispatch<MouseFocusWindowChangedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnMouseFocusWindowChanged));
		dispatcher.Dispatch<DeviceChangedEvent>(SG_BIND_EVENT_FUNC(ImGuiLayer::OnDeviceChanged));
	}

	bool ImGuiLayer::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}

	bool ImGuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto hwnd = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		int button = 0;
		auto msg = e.GetKeycode();
		if (msg == VK_LBUTTON) { button = 0; }
		if (msg == VK_RBUTTON) { button = 1; }
		if (msg == VK_MBUTTON) { button = 2; }
		if (msg == VK_XBUTTON1) { button = 3; }
		else if (msg == VK_XBUTTON2) { button = 4; }
		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
			::SetCapture(hwnd);
		io.MouseDown[button] = true;
		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto hwnd = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		int button = 0;
		auto msg = e.GetKeycode();
		if (msg == VK_LBUTTON) { button = 0; }
		if (msg == VK_RBUTTON) { button = 1; }
		if (msg == VK_MBUTTON) { button = 2; }
		if (msg == VK_XBUTTON1) { button = 3; }
		else if (msg == VK_XBUTTON2) { button = 4; }
		io.MouseDown[button] = false;
		if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
			::ReleaseCapture();
		return false;
	}

	bool ImGuiLayer::OnMouseScrolled(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto offsetX = e.GetScrollX();
		auto offsetY = e.GetScrollY();
		if (offsetX != 0.0f) io.MouseWheelH += offsetX;
		if (offsetY != 0.0f) io.MouseWheel += offsetY;
		return false;
	}

	bool ImGuiLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto button = e.GetKeycode();
		io.KeysDown[button] = 1;
		return false;
	}

	bool ImGuiLayer::OnKeyReleased(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto button = e.GetKeycode();
		io.KeysDown[button] = 0;
		return false;
	}

	bool ImGuiLayer::OnKeyTyped(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto charactor = e.GetKeycode();
		io.AddInputCharacterUTF16((unsigned int)charactor);
		return false;
	}

	bool ImGuiLayer::OnMouseFocusWindowChanged(MouseFocusWindowChangedEvent& e)
	{
		auto pos = e.GetCursorPosition();
		if (pos == HTCLIENT)
			ImGui_ImplWin32_UpdateMouseCursor();
		return false;
	}

	bool ImGuiLayer::OnDeviceChanged(DeviceChangedEvent& e)
	{
		auto msg = e.GetMessage();
		//if (msg == DBT_DEVNODES_CHANGED)
		//	g_WantUpdateHasGamepad = true;
		return false;
	}

}