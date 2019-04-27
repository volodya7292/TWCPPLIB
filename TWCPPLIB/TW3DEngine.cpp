#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DDevice.h"
#include "TW3DSwapChain.h"
#include "TW3DShaders.h"
#include "TW3DModules.h"
#include "TW3DPrimitives.h"
#include "TWLogger.h"

static TW::TWLogger* logger;

static TWT::Float delta_time;

static TW3D::DefaultHandler       on_update, on_cleanup;
static TW3D::ThreadTickHandler    on_thread_tick;
static TW3D::KeyHandler           on_key_down, on_key_up;
static TW3D::CharHandler          on_char;

static TWT::Vector<TWT::Bool> KeysDown(1024);

static const TWT::UInt engine_thread_count = 1;

static TWT::Vector<std::thread>    threads;
static std::mutex                  resize_mutex;

static TW3D::TW3DRenderer*           renderer;
static TW3D::TW3DResourceManager*    resource_manager;

static TWT::UInt      width, height;
static TWT::String    title;
static TWT::Bool      fullscreen = false;
static TWT::UInt      additional_thread_count;

static TWT::Bool      running, minimized = false;
static HWND           hwnd;

static TWT::UInt      current_frame_index;

static TW3D::TW3DFactory*      factory;
static TW3D::TW3DAdapter*      adapter;
static TW3D::TW3DDevice*       device;
static TW3D::TW3DSwapChain*    swapChain;

static TWT::Vector<TW3D::TW3DResourceRTV*>    renderTargets(TW3D::TW3DSwapChain::BufferCount);
static TW3D::TW3DResourceDSV*                 depthStencil;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void init_window() {
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

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	logger->LogInfo("Window initialized.");
}

void init_input() {
	for (size_t i = 0; i < KeysDown.size(); i++)
		KeysDown[i] = false;
	logger->LogInfo("User input interface initialized.");
}

void init_dx12() {
	TWT::UInt dxgi_factory_flags = 0;

#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
			debugController->EnableDebugLayer();
			ComPtr<ID3D12Debug1> debugController1;
			debugController->QueryInterface(IID_PPV_ARGS(&debugController1));
			//debugController1->SetEnableGPUBasedValidation(true);
		} else
			OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");

		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf())))) {
			dxgi_factory_flags = DXGI_CREATE_FACTORY_DEBUG;
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}
	}
#endif

	logger->LogInfo("DirectX debug layer initialized.");

	factory = new TW3D::TW3DFactory(dxgi_factory_flags);
	logger->LogInfo("DirectX initialization stage 1");
	std::vector<TW3D::TW3DAdapter*> adapters = TW3D::TW3DAdapter::ListAvailable(factory, D3D_FEATURE_LEVEL_11_0);
	adapter = adapters[0];
	logger->LogInfo("DirectX initialization stage 2");
	device = new TW3D::TW3DDevice(adapter);
	logger->LogInfo("DirectX initialization stage 3");
	resource_manager = new TW3D::TW3DResourceManager(device);
	logger->LogInfo("DirectX initialization stage 4");
	swapChain = new TW3D::TW3DSwapChain(factory, resource_manager->GetDirectCommandQueue(), hwnd, width, height, true);
	logger->LogInfo("DirectX initialization stage 5");

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++)
		renderTargets[i] = resource_manager->CreateRenderTargetView(swapChain->GetBuffer(i));
	logger->LogInfo("DirectX initialization stage 6");

	depthStencil = resource_manager->CreateDepthStencilView(width, height);
	logger->LogInfo("DirectX initialization stage 7");

	current_frame_index = swapChain->GetCurrentBufferIndex();
	logger->LogInfo("DirectX initialization stage 8");
	TW3DPrimitives::Initialize(resource_manager);
	TW3DShaders::Initialize(resource_manager);
	TW3DModules::Initialize(resource_manager);
	logger->LogInfo("DirectX initialization stage 9");

	logger->LogInfo("DirectX initialized.");
}

void update() {
	renderer->Update();
	if (on_update)
		on_update();
}

void render() {
	renderer->RecordBeforeExecution();
	renderer->Execute(swapChain->GetCurrentBufferIndex());
	swapChain->Present();
}

TWT::UInt thread_tick(TWT::UInt thread_id, TWT::UInt thread_count) {
	if (thread_id == 0) { // Command list record
		synchronized(resize_mutex) {
			for (size_t i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++)
				renderer->Record(i, renderTargets[i], depthStencil);
			renderer->AdjustRecordIndex();
		}
		return 30;
	} else {
		if (on_thread_tick)
			return on_thread_tick(thread_id - engine_thread_count, thread_count - engine_thread_count);
	}

	return 50;
}

void on_resize() {
	resource_manager->FlushCommandLists();

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

		if (renderer)
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
			TWT::Float64 t0 = TWU::GetTimeSeconds();
			update();
			if (!minimized)
				render();
			TWT::Float64 t1 = TWU::GetTimeSeconds();
			delta_time = t1 - t0;
		}
	}

	for (size_t i = 0; i < total_thread_count; i++)
		threads[i].join();

	resource_manager->FlushCommandLists();
}

void cleanup() {
	if (swapChain->GetFullscreen())
		swapChain->SetFullscreen(false);

	delete factory;
	delete adapter;
	delete device;
	delete swapChain;
	delete depthStencil;
	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; ++i)
		delete renderTargets[i];

	TW3DPrimitives::Release();
	TW3DShaders::Release();
	TW3DModules::Release();

	if (on_cleanup)
		on_cleanup();

	delete resource_manager;
	delete logger;

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
#endif // _DEBUG
}

void TW3D::Initialize(const InitializeInfo& info) {
	width = info.WindowWidth;
	height = info.WindowHeight;
	title = info.WindowTitle;
	additional_thread_count = info.AdditionalThreadCount;

#ifdef _DEBUG
	logger = new TW::TWLogger("TW3D");
#else
	logger = new TW::TWLogger(info.LogFilename, "TW3D");
#endif // _DEBUG

	init_window();
	init_input();
	init_dx12();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_KEYDOWN:
		if (!KeysDown[wParam]) {
			KeysDown[wParam] = true;
			if (on_key_down)
				on_key_down(wParam);
		}
		return 0;

	case WM_KEYUP:
		if (KeysDown[wParam]) {
			KeysDown[wParam] = false;
			if (on_key_up)
				on_key_up(wParam);
		}
		return 0;

	case WM_CHAR:
		if (on_char)
			on_char(wParam);
		return 0;

	case WM_SIZE:
		if ((UINT)wParam == SIZE_MAXIMIZED)
			on_resize();
		else if ((UINT)wParam == SIZE_MINIMIZED)
			minimized = true;
		else if ((UINT)wParam == SIZE_RESTORED) // unminimized
			minimized = false;

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

void TW3D::Shutdown() {
	running = false;
	DestroyWindow(hwnd);
}

TWT::Bool TW3D::GetFullScreen() {
	return fullscreen;
}

void TW3D::SetFullScreen(TWT::Bool FullScreen) {
	if (fullscreen != FullScreen) {
		fullscreen = FullScreen;
		swapChain->SetFullscreen(fullscreen);
		on_resize();
	}
}

TWT::Bool TW3D::GetVSync() {
	return swapChain->VSync;
}

TWT::Bool TW3D::IsKeyDown(TWT::UInt KeyCode) {
	return KeysDown[KeyCode];
}

TWT::Vector2u TW3D::GetCursorPosition() {
	POINT pos;
	GetCursorPos(&pos);
	return TWT::Vector2u(pos.x, pos.y);
}

TWT::Vector2u TW3D::GetWindowPosition() {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	RECT rect2 = {};
	AdjustWindowRect(&rect2, WS_OVERLAPPEDWINDOW, FALSE);
	return TWT::Vector2u(rect.left - rect2.left, rect.top - rect2.top);
}

TWT::Vector2u TW3D::GetWindowCenterPosition() {
	return GetWindowPosition() + TWT::Vector2u(width, height) / 2u;
}

TWT::Vector2u TW3D::GetCurrentWindowSize() {
	return TWT::Vector2u(width, height);
}

TWT::Float TW3D::GetFPS() {
	return 1.0f / delta_time;
}

TWT::Float TW3D::GetDeltaTime() {
	return delta_time;
}

void TW3D::SetVSync(TWT::Bool VSync) {
	swapChain->VSync = VSync;
}

void TW3D::SetWindowTitle(TWT::String WindowTitle) {
	title = WindowTitle;
	TWT::WString wstrtitle = title.Wide();
	const TWT::WChar* wtitle = wstrtitle.data.c_str();
	SetWindowText(hwnd, wtitle);
}

void TW3D::SetRenderer(TW3DRenderer* Renderer) {
	renderer = Renderer;
	renderer->Initialize(resource_manager, swapChain, width, height);
	renderer->Resize(width, height);
}

void TW3D::SetOnUpdateEvent(DefaultHandler OnUpdate) {
	on_update = OnUpdate;
}

void TW3D::SetOnCleanupEvent(DefaultHandler OnCleanup) {
	on_cleanup = OnCleanup;
}

void TW3D::SetOnThreadTickEvent(ThreadTickHandler OnThreadTick) {
	on_thread_tick = OnThreadTick;
}

void TW3D::SetOnKeyDownEvent(KeyHandler OnKeyDown) {
	on_key_down = OnKeyDown;
}

void TW3D::SetOnKeyUpEvent(KeyHandler OnKeyUp) {
	on_key_up = OnKeyUp;
}

void TW3D::SetOnCharEvent(CharHandler OnChar) {
	on_char = OnChar;
}

TW3D::TW3DResourceManager* TW3D::GetResourceManager() {
	return resource_manager;
}
