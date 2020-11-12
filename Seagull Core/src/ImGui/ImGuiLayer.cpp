#include "sgpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Platform/DirectX/DirectX12RendererAPI.h"

// Forward declare message handler from imgui_impl_win32.cpp
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SG
{

	void ImGuiLayer::OnAttach()
	{
		ImGui_ImplWin32_EnableDpiAwareness();
		HWND hwnd = static_cast<HWND>(Application::Get().GetWindow().GetNativeWindow());
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

	void ImGuiLayer::OnEvent(Event& e)
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
		auto instance = (DirectX12RendererAPI*)RenderCommand::GetRenderAPIInstance();

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

//
//	// Win32 message handler
//	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//	{
//		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//			return true;
//
//		switch (msg)
//		{
//		case WM_SIZE:
//			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
//			{
//				WaitForLastSubmittedFrame();
//				ImGui_ImplDX12_InvalidateDeviceObjects();
//				CleanupRenderTarget();
//				ResizeSwapChain(hWnd, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
//				CreateRenderTarget();
//				ImGui_ImplDX12_CreateDeviceObjects();
//			}
//			return 0;
//		case WM_SYSCOMMAND:
//			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
//				return 0;
//			break;
//		case WM_DESTROY:
//			::PostQuitMessage(0);
//			return 0;
//		}
//		return ::DefWindowProc(hWnd, msg, wParam, lParam);
//	}

}