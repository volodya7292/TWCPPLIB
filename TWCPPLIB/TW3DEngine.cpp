#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DDevice.h"
#include "TW3DSwapChain.h"
#include "TW3DFence.h"
#include "TW3DResourceDSV.h"
#include "TW3DResourceRTV.h"
#include "TW3DResourceUAV.h"
#include "TW3DResourceSR.h"
#include "TW3DResourceVB.h"
#include "TW3DTempGCL.h"
#include "TW3DGraphicsPipelineState.h"
#include "TW3DComputePipelineState.h"
#include "TW3DRootSignature.h"
#include "TW3DResourceCB.h"
#include "TW3DCube.h"
#include "TW3DPrimitives.h"
#include "TW3DPerspectiveCamera.h"

static void (*on_update)() = nullptr;
static void (*on_cleanup)() = nullptr;
static TWT::UInt(*on_thread_tick)(TWT::UInt ThreadID, TWT::UInt ThreadCount);

static const TWT::UInt engine_thread_count = 1;

static TWT::Vector<std::thread>    threads;
static std::mutex                  resize_mutex;

static TW3D::TW3DRenderer*           renderer;
static TW3D::TW3DResourceManager*    resource_manager;

static TWT::UInt      width, height;
static TWT::String    title;
static TWT::Bool      fullscreen, vsync;
static TWT::UInt      additional_thread_count;

static TWT::Bool      running, minimized = false;
static HWND           hwnd;

static TWT::UInt      current_frame_index;

static TW3D::TW3DFactory*      factory;
static TW3D::TW3DAdapter*      adapter;
static TW3D::TW3DDevice*       device;
static TW3D::TW3DSwapChain*    swapChain;

static TWT::Vector<TW3D::TW3DResourceRTV*> renderTargets(TW3D::TW3DSwapChain::BufferCount);

static TW3D::TW3DFence*          fence;
static TW3D::TW3DResourceDSV*    depthStencil;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void init_window() {
	if (fullscreen) {
		HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		width = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	TWT::WString wstrtitle = title.Wide();
	const TWT::WChar* wtitle = wstrtitle.data.c_str();

	WNDCLASSEX wc;
	HINSTANCE instance = GetModuleHandle(NULL);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = wtitle;
	wc.hIconSm = wc.hIcon;
	if (!RegisterClassEx(&wc)) {
		std::cout << GetLastError();
		MessageBox(NULL, L"sd", L"sd", MB_OK | MB_ICONERROR);
	}

	TWT::UInt x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	TWT::UInt y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	RECT rect = { (LONG)x, (LONG)y, (LONG)(x + width), (LONG)(y + height) };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hwnd = CreateWindowEx(NULL,
		wtitle,
		wtitle,
		WS_OVERLAPPEDWINDOW,
		rect.left, rect.top,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL,
		NULL,
		instance,
		NULL);

	if (!hwnd) {
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	if (fullscreen)
		SetWindowLong(hwnd, GWL_STYLE, 0);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void init_dx12() {
	TWT::UInt dxgi_factory_flags = 0;

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
			debugController->EnableDebugLayer();
		else
			OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf())))) {
			dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif

	factory = new TW3D::TW3DFactory(dxgi_factory_flags);
	std::vector<TW3D::TW3DAdapter*> adapters = TW3D::TW3DAdapter::ListAvailable(factory, D3D_FEATURE_LEVEL_11_0);
	adapter = adapters[0];
	device = new TW3D::TW3DDevice(adapter);
	resource_manager = new TW3D::TW3DResourceManager(device);
	swapChain = new TW3D::TW3DSwapChain(factory, resource_manager->GetDirectCommandQueue(), hwnd, width, height, vsync);
	fence = new TW3D::TW3DFence(device);

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++)
		renderTargets[i] = resource_manager->CreateRenderTargetView(swapChain->GetBuffer(i));

	// Create the depth/stencil buffer
	depthStencil = resource_manager->CreateDepthStencilView(width, height);

	current_frame_index = swapChain->GetCurrentBufferIndex();
	TW3DPrimitives::Initialize(resource_manager);
}

void FlushGPU() {
	resource_manager->Flush(fence);
}

void update() {
	renderer->Update();
	if (on_update)
		on_update();
}

void render() {
	FlushGPU();
	renderer->Execute(swapChain->GetCurrentBufferIndex());
	swapChain->Present();
}

TWT::UInt thread_tick(TWT::UInt thread_id, TWT::UInt thread_count) {
	if (thread_id == 0) { // Command list record
		FlushGPU();
		synchronized(resize_mutex) {
			for (size_t i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++)
				renderer->Record(i, renderTargets[i], depthStencil);
			renderer->AdjustRecordIndex();
		}
		return 30;
	} else {
		if (on_thread_tick)
			return on_thread_tick(thread_id, thread_count);
	}

	return 50;
}

void on_resize() {
	FlushGPU();

	synchronized(resize_mutex) {
		RECT clientRect = {};
		GetClientRect(hwnd, &clientRect);
		width = std::max(clientRect.right - clientRect.left, 1L);
		height = std::max(clientRect.bottom - clientRect.top, 1L);

		for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
			renderTargets[n]->Release();
		depthStencil->Release();

		swapChain->Resize(width, height);
		current_frame_index = swapChain->GetCurrentBufferIndex();

		for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
			renderTargets[n]->Create(swapChain->GetBuffer(n));
		depthStencil->Create(width, height);

		renderer->Resize(width, height);
	}
}

void main_loop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	running = true;

	TWT::UInt total_thread_count = engine_thread_count + additional_thread_count;
	for (size_t i = 0; i < total_thread_count; i++) {
		std::thread thread([i, total_thread_count]() {
			while (running) {
				TWT::UInt millis = thread_tick(static_cast<TWT::UInt>(i), total_thread_count);
				Sleep(millis);
			}
			});
		threads.push_back(std::move(thread));
	}

	while (running) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			update();
			if (!minimized)
				render();
		}
	}

	for (size_t i = 0; i < total_thread_count; i++)
		threads[i].join();

	FlushGPU();
}

void cleanup() {
	if (swapChain->GetFullscreen())
		swapChain->SetFullscreen(false);

	delete factory;
	delete adapter;
	delete device;
	delete swapChain;
	delete resource_manager;
	delete depthStencil;
	delete fence;

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; ++i)
		delete renderTargets[i];

	TW3DPrimitives::Cleanup();

	if (on_cleanup)
		on_cleanup();

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
#endif // _DEBUG
}

void TW3D::Initialize(const InitializeInfo& info) {
	width = info.Width;
	height = info.Height;
	title = info.Title;
	fullscreen = info.FullScreen;
	vsync = info.VSync;

	init_window();
	init_dx12();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			running = false;
			DestroyWindow(hwnd);
		} else if (wParam == VK_F11) {
			fullscreen = !fullscreen;
			swapChain->SetFullscreen(fullscreen);
			on_resize();
		}
		return 0;

	case WM_SIZE:
		if ((UINT)wParam == SIZE_MAXIMIZED) {
			on_resize();
		} else if ((UINT)wParam == SIZE_MINIMIZED) {
			minimized = true;
		} else if ((UINT)wParam == SIZE_RESTORED) { // unminimized
			minimized = false;
		}

		if (swapChain) {
			TWT::Bool govno = swapChain->GetFullscreen();
			if (fullscreen != govno) {
				fullscreen = govno;
				on_resize();
			}
		}

		return 0;

	case WM_EXITSIZEMOVE:
		if ((UINT)wParam != SIZE_MAXIMIZED)
			on_resize();
		return 0;

	case WM_DESTROY: // X button on top right corner of window was pressed
		running = false;
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void TW3D::Start() {
	main_loop();
	cleanup();
}

void TW3D::SetRenderer(TW3DRenderer* Renderer) {
	renderer = Renderer;
	renderer->Initialize(resource_manager, swapChain, width, height);
}

void TW3D::SetOnUpdateEvent(void(*OnUpdate)()) {
	on_update = OnUpdate;
}

void TW3D::SetOnThreadTickEvent(TWT::UInt(*OnThreadTick)(TWT::UInt ThreadID, TWT::UInt ThreadCount)) {
	on_thread_tick = OnThreadTick;
}

void TW3D::SetOnCleanupEvent(void(*OnCleanup)()) {
	on_cleanup = OnCleanup;
}

TW3D::TW3DResourceManager* TW3D::GetResourceManager() {
	return resource_manager;
}
