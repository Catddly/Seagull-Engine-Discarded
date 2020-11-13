#include "sgpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

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
		ImGui_ImplWin32_EnableDpiAwareness();
		HWND hwnd = static_cast<HWND>(Application::Get().GetWindow()->GetNativeWindow());
		auto instance = (DirectX12RendererAPI*)RenderCommand::GetRenderAPIInstance();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX12_Init(instance->m_D3dDevice.Get(), 2,
			DXGI_FORMAT_R8G8B8A8_UNORM, instance->m_SrvHeap->GetNative(),
			instance->m_SrvHeap->GetCPUStart(),
			instance->m_SrvHeap->GetGPUStart());
	}

	void ImGuiLayer::OnDettach()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
	}

	void ImGuiLayer::OnUpdate()
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
		//ImGuiIO& io = ImGui::GetIO();
		//io.Fonts->TexID = NULL; // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
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
		if (pos == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
			return true;
		return false;
	}

	bool ImGuiLayer::OnDeviceChanged(DeviceChangedEvent& e)
	{
		auto msg = e.GetMessage();
		//if (msg == DBT_DEVNODES_CHANGED)
		//	g_WantUpdateHasGamepad = true;
		return false;
	}

	//// Win32 message handler
	//LRESULT WINAPI ImGuiLayer::ImGuiWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	//{
	//	if (ImGui::GetCurrentContext() == NULL)
	//		return 0;

	//	ImGuiIO& io = ImGui::GetIO();

	//	switch (msg)
	//	{
	//		//case WM_SIZE:
	//		//{
	//		//	RenderCommand::SetViewportSize((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
	//		//	return 0;
	//		//}
	//		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	//		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	//		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	//		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	//		{
	//			int button = 0;
	//			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
	//			if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
	//			if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
	//			if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
	//			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
	//				::SetCapture(hwnd);
	//			io.MouseDown[button] = true;
	//			return 0;
	//		}
	//		case WM_LBUTTONUP:
	//		case WM_RBUTTONUP:
	//		case WM_MBUTTONUP:
	//		case WM_XBUTTONUP:
	//		{
	//			int button = 0;
	//			if (msg == WM_LBUTTONUP) { button = 0; }
	//			if (msg == WM_RBUTTONUP) { button = 1; }
	//			if (msg == WM_MBUTTONUP) { button = 2; }
	//			if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4; }
	//			io.MouseDown[button] = false;
	//			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
	//				::ReleaseCapture();
	//			return 0;
	//		}
	//		case WM_MOUSEWHEEL:
	//			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
	//			return 0;
	//		case WM_MOUSEHWHEEL:
	//			io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
	//			return 0;
	//		case WM_KEYDOWN:
	//		case WM_SYSKEYDOWN:
	//			if (wParam < 256)
	//				io.KeysDown[wParam] = 1;
	//			return 0;
	//		case WM_KEYUP:
	//		case WM_SYSKEYUP:
	//			if (wParam < 256)
	//				io.KeysDown[wParam] = 0;
	//			return 0;
	//		case WM_CHAR:
	//			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
	//			if (wParam > 0 && wParam < 0x10000)
	//				io.AddInputCharacterUTF16((unsigned short)wParam);
	//			return 0;
	//		case WM_SETCURSOR:
	//			if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
	//				return 1;
	//			return 0;
	//		case WM_DEVICECHANGE:
	//			if ((UINT)wParam == DBT_DEVNODES_CHANGED)
	//				g_WantUpdateHasGamepad = true;
	//			return 0;
	//	}
	//	return ::DefWindowProc(hWnd, msg, wParam, lParam);
	//}

}