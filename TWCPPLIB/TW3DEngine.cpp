#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DSwapChain.h"
#include "TW3DSCFrame.h"
#include "TW3DShaders.h"
#include "TW3DModules.h"
#include "TW3DPrimitives.h"
#include "TWLogger.h"

static TW::TWLogger* logger;

static float delta_time = 1.0f / 60.0f;

static TW3D::DefaultHandler       on_update, on_cleanup;
static TW3D::ThreadTickHandler    on_thread_tick;
static TW3D::KeyHandler           on_key;
static TW3D::CharHandler          on_char;

static std::vector<bool> KeysDown(1024);

static const TWT::uint engine_thread_count = 2;
static const TWT::uint cl_record_thread_count = TW3DSwapChain::BufferCount;
static std::vector<std::thread>    threads;
static std::mutex                  resize_sync;

static TW3DRenderer*           renderer;
static TW3DResourceManager*    resource_manager;

static TWT::uint      width, height;
static TWT::String    title;
static bool           fullscreen = false;
static TWT::uint      additional_thread_count;

static bool    initialized = false, running, minimized = false;
static HWND    hwnd;

static TW3DFactory*      factory;
static TW3DAdapter*      adapter;
static TW3DDevice*       device;
static TW3DSwapChain*    swapChain;

static std::vector<TW3DSCFrame*>    frames(TW3DSwapChain::BufferCount);
//static std::vector<TW3DRenderTarget*> renderTargets(TW3DSwapChain::BufferCount);
//static TW3DTexture*                 depthStencil;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void init_window() {
	TWT::WString wstrtitle = title.Wide();
	const TWT::wchar* wtitle = wstrtitle.ToWCharArray();

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

	TWT::uint x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	TWT::uint y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
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
	TWT::uint dxgi_factory_flags = 0;

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

		logger->LogInfo("DirectX debug layer initialized.");
	}
#endif

	factory = new TW3DFactory(dxgi_factory_flags);
	logger->LogInfo("DirectX Stage 1 initialized");
	std::vector<TW3DAdapter*> adapters = TW3DAdapter::ListAvailable(factory, D3D_FEATURE_LEVEL_12_0);
	if (adapters.size() == 0) {
		logger->LogError("[DirectX Stage 2]: No suitable device found.");
		return;
	}
	adapter = adapters[0];
	logger->LogInfo("DirectX Stage 2 initialized");
	device = new TW3DDevice(adapter);
	logger->LogInfo("DirectX Stage 3 initialized");
	resource_manager = new TW3DResourceManager(device);
	logger->LogInfo("DirectX Stage 4 initialized");
	swapChain = new TW3DSwapChain(factory, resource_manager->GetDirectCommandQueue(), hwnd, width, height, true);
	logger->LogInfo("DirectX Stage 5 initialized");

	for (int i = 0; i < TW3DSwapChain::BufferCount; i++)
		frames[i] = new TW3DSCFrame(resource_manager, i, swapChain->GetBuffer(i));
	logger->LogInfo("DirectX Stage 6 initialized");

	logger->LogInfo("DirectX Stage 8 initialized");
	TW3DPrimitives::Initialize(resource_manager);
	TW3DShaders::Initialize(resource_manager);
	TW3DModules::Initialize(resource_manager);
	logger->LogInfo("DirectX Stage 9 initialized");

	D3D12_FEATURE_DATA_D3D12_OPTIONS opt0 = {};
	device->GetFeatureData(D3D12_FEATURE_D3D12_OPTIONS, &opt0, sizeof(opt0));
	D3D12_FEATURE_DATA_D3D12_OPTIONS1 opt1 = {};
	device->GetFeatureData(D3D12_FEATURE_D3D12_OPTIONS1, &opt1, sizeof(opt1));
	D3D12_FEATURE_DATA_D3D12_OPTIONS2 opt2 = {};
	device->GetFeatureData(D3D12_FEATURE_D3D12_OPTIONS2, &opt2, sizeof(opt2));
	D3D12_FEATURE_DATA_D3D12_OPTIONS3 opt3 = {};
	device->GetFeatureData(D3D12_FEATURE_D3D12_OPTIONS3, &opt3, sizeof(opt3));
	D3D12_FEATURE_DATA_D3D12_OPTIONS4 opt4 = {};
	device->GetFeatureData(D3D12_FEATURE_D3D12_OPTIONS4, &opt4, sizeof(opt4));
	D3D12_FEATURE_DATA_D3D12_OPTIONS5 opt5 = {};
	device->GetFeatureData(D3D12_FEATURE_D3D12_OPTIONS5, &opt5, sizeof(opt5));


	logger->LogInfo("DirectX initialized.");
	logger->LogInfo("----------------------------------------------------------------------------------");
	logger->LogInfo("Using DirectX 12 on "s + adapter->GetDescription().Multibyte() + "."s);
	logger->LogInfo("----------------------------------------------------------------------------------");
	logger->LogInfo("DoublePrecisionFloatShaderOps        : "s + TWU::BoolStr(opt0.DoublePrecisionFloatShaderOps));
	logger->LogInfo("OutputMergerLogicOp                  : "s + TWU::BoolStr(opt0.OutputMergerLogicOp));
	logger->LogInfo("PSSpecifiedStencilRefSupported       : "s + TWU::BoolStr(opt0.PSSpecifiedStencilRefSupported));
	logger->LogInfo("TypedUAVLoadAdditionalFormats        : "s + TWU::BoolStr(opt0.TypedUAVLoadAdditionalFormats));
	logger->LogInfo("ROVsSupported                        : "s + TWU::BoolStr(opt0.ROVsSupported));
	logger->LogInfo("StandardSwizzle64KBSupported         : "s + TWU::BoolStr(opt0.StandardSwizzle64KBSupported));
	logger->LogInfo("CrossAdapterRowMajorTextureSupported : "s + TWU::BoolStr(opt0.CrossAdapterRowMajorTextureSupported));
	logger->LogInfo("VPAndRTArrayIndex...Supported...     : "s + TWU::BoolStr(opt0.VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation));
	logger->LogInfo("WaveOps                              : "s + TWU::BoolStr(opt1.WaveOps));
	logger->LogInfo("ExpandedComputeResourceStates        : "s + TWU::BoolStr(opt1.ExpandedComputeResourceStates));
	logger->LogInfo("Int64ShaderOps                       : "s + TWU::BoolStr(opt1.Int64ShaderOps));
	logger->LogInfo("DepthBoundsTestSupported             : "s + TWU::BoolStr(opt2.DepthBoundsTestSupported));
	logger->LogInfo("CopyQueueTimestampQueriesSupported   : "s + TWU::BoolStr(opt3.CopyQueueTimestampQueriesSupported));
	logger->LogInfo("CastingFullyTypedFormatSupported     : "s + TWU::BoolStr(opt3.CastingFullyTypedFormatSupported));
	logger->LogInfo("BarycentricsSupported                : "s + TWU::BoolStr(opt3.BarycentricsSupported));
	logger->LogInfo("MSAA64KBAlignedTextureSupported      : "s + TWU::BoolStr(opt4.MSAA64KBAlignedTextureSupported));
	logger->LogInfo("Native16BitShaderOpsSupported        : "s + TWU::BoolStr(opt4.Native16BitShaderOpsSupported));
	logger->LogInfo("SRVOnlyTiledResourceTier3            : "s + TWU::BoolStr(opt5.SRVOnlyTiledResourceTier3));
	logger->LogInfo("MaxGPUVirtualAddressBitsPerResource  : "s + opt0.MaxGPUVirtualAddressBitsPerResource);
	logger->LogInfo("WaveLaneCountMin                     : "s + opt1.WaveLaneCountMin);
	logger->LogInfo("WaveLaneCountMax                     : "s + opt1.WaveLaneCountMax);
	logger->LogInfo("TotalLaneCount                       : "s + opt1.TotalLaneCount);
	logger->LogInfo("MinPrecisionSupport                  : "s + static_cast<TWT::uint>(opt0.MinPrecisionSupport));
	logger->LogInfo("TiledResourcesTier                   : "s + static_cast<TWT::uint>(opt0.TiledResourcesTier));
	logger->LogInfo("ResourceBindingTier                  : "s + static_cast<TWT::uint>(opt0.ResourceBindingTier));
	logger->LogInfo("ConservativeRasterizationTier        : "s + static_cast<TWT::uint>(opt0.ConservativeRasterizationTier));
	logger->LogInfo("CrossNodeSharingTier                 : "s + static_cast<TWT::uint>(opt0.CrossNodeSharingTier));
	logger->LogInfo("ResourceHeapTier                     : "s + static_cast<TWT::uint>(opt0.ResourceHeapTier));
	logger->LogInfo("ProgrammableSamplePositionsTier      : "s + static_cast<TWT::uint>(opt2.ProgrammableSamplePositionsTier));
	logger->LogInfo("ViewInstancingTier                   : "s + static_cast<TWT::uint>(opt3.ViewInstancingTier));
	logger->LogInfo("SharedResourceCompatibilityTier      : "s + static_cast<TWT::uint>(opt4.SharedResourceCompatibilityTier));
	logger->LogInfo("RenderPassesTier                     : "s + static_cast<TWT::uint>(opt5.RenderPassesTier));
	logger->LogInfo("RaytracingTier                       : "s + static_cast<TWT::uint>(opt5.RaytracingTier));
	logger->LogInfo("WriteBufferImmediateSupportFlags     : "s + TWU::DXCommandListSupportFlagsStr(opt3.WriteBufferImmediateSupportFlags));

	logger->LogInfo("----------------------------------------------------------------------------------");

	initialized = true;
}

void update() {
	renderer->Update(delta_time);
	if (on_update)
		on_update();
}

void render() {
	synchronized(resize_sync) {
		TWT::uint curr_index = swapChain->GetCurrentBufferIndex();

		frames[(curr_index + TW3DSwapChain::BufferCount - 1) % TW3DSwapChain::BufferCount]->FlushCommandLists(); // flush previous frame command lists
		renderer->Execute(frames[curr_index]);
	}

	swapChain->Present();
}

TWT::uint thread_tick(TWT::uint thread_id, TWT::uint thread_count) {
	if (thread_id < cl_record_thread_count) { // Command list record
		frames[thread_id]->RecordCommandLists();
		return 30;
	} else if (thread_id == cl_record_thread_count + 1) {
		HRESULT remove_reason = device->GetRemoveReason();
		if (FAILED(remove_reason)) {
			logger->LogError("Device removed: "s + TWU::HResultToWString(remove_reason).Multibyte());
			running = false;
		}
		return static_cast<TWT::uint>(delta_time * 1000.0f);
	} else {
		if (on_thread_tick)
			return on_thread_tick(thread_id - engine_thread_count, thread_count - engine_thread_count);
	}

	return 50;
}

void on_resize() {
	resource_manager->FlushCommandLists();

	logger->LogInfo("[on_resize] Resizing started."s);

	synchronized(resize_sync) {
		RECT clientRect = {};
		GetClientRect(hwnd, &clientRect);
		width = TWT::Max(clientRect.right - clientRect.left, 1L);
		height = TWT::Max(clientRect.bottom - clientRect.top, 1L);

		for (UINT i = 0; i < TW3DSwapChain::BufferCount; i++)
			frames[i]->Release();

		swapChain->Resize(width, height);

		logger->LogInfo("[on_resize] Render target resized to "s + width + "x"s + height);

		if (renderer) {
			renderer->Resize(TWT::uint2(width, height));
			logger->LogInfo("[on_resize] Renderer resized to "s + width + "x"s + height);
		}

		for (UINT i = 0; i < TW3DSwapChain::BufferCount; i++) {
			frames[i]->RenderTarget->Create(swapChain->GetBuffer(i));
			renderer->InitializeFrame(frames[i]);
		}
	}

	logger->LogInfo("[on_resize] Resizing finished."s);
}

void main_loop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	running = true;

	TWT::uint total_thread_count = engine_thread_count + cl_record_thread_count + additional_thread_count;
	for (size_t i = 0; i < total_thread_count; i++) {
		std::thread thread([i, total_thread_count]() {
			while (running) {
				TWT::uint millis = thread_tick(static_cast<TWT::uint>(i), total_thread_count);
				Sleep(millis);
			}
		});
		threads.push_back(std::move(thread));
	}
	logger->LogInfo("Threads initialized."s);

	while (running) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			double t0 = TWU::GetTimeSeconds();
			update();
			if (!minimized)
				render();
			double t1 = TWU::GetTimeSeconds();
			delta_time = t1 - t0;
		}
	}

	for (size_t i = 0; i < total_thread_count; i++)
		threads[i].join();
	logger->LogInfo("Threads joined."s);

	resource_manager->FlushCommandLists();
	logger->LogInfo("Main loop finished."s);
}

void cleanup() {
	if (swapChain->GetFullScreen())
		swapChain->SetFullScreen(false);

	delete factory;
	delete adapter;
	delete device;
	delete swapChain;

	for (int i = 0; i < TW3DSwapChain::BufferCount; ++i)
		delete frames[i];

	TW3DModules::Release();

	if (on_cleanup)
		on_cleanup();

	delete resource_manager;

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
#endif // _DEBUG

	logger->LogInfo("Engine cleaned up."s);

	delete logger;
}

void TW3D::Initialize(const TW3D::InitializeInfo& info) {
	width = info.WindowWidth;
	height = info.WindowHeight;
	title = info.WindowTitle;
	additional_thread_count = info.AdditionalThreadCount;

#ifdef _DEBUG
	logger = new TW::TWLogger("TW3D");
#else
	logger = new TW::TWLogger(info.LogFilename, "TW3D");
#endif // _DEBUG

	TWU::TW3DSetLogger(logger);

	init_window();
	init_input();
	init_dx12();

	logger->LogInfo("Engine initialized."s);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_KEYDOWN:
		if (!KeysDown[wParam]) {
			KeysDown[wParam] = true;
			if (on_key)
				on_key(wParam, TW3D::KEY_ACTION_DOWN);
		}
		return 0;

	case WM_KEYUP:
		if (KeysDown[wParam]) {
			KeysDown[wParam] = false;
			if (on_key)
				on_key(wParam, TW3D::KEY_ACTION_UP);
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
			bool govno = swapChain->GetFullScreen();
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
	if (initialized) {
		main_loop();
		cleanup();
	}
}

void TW3D::Shutdown() {
	logger->LogInfo("Engine shutdown."s);

	if (!running && initialized)
		cleanup();
	running = false;

	DestroyWindow(hwnd);
}

bool TW3D::GetFullScreen() {
	return fullscreen;
}

void TW3D::SetFullScreen(bool FullScreen) {
	if (fullscreen != FullScreen) {
		fullscreen = FullScreen;
		swapChain->SetFullScreen(fullscreen);
		logger->LogInfo("Fullscreen: "s + (fullscreen ? "On"s : "Off"s));
		on_resize();
	}
}

bool TW3D::GetVSync() {
	return swapChain->VSync;
}

bool TW3D::IsKeyDown(TWT::uint KeyCode) {
	return KeysDown[KeyCode];
}

TWT::uint2 TW3D::GetCursorPosition() {
	POINT pos;
	GetCursorPos(&pos);
	return TWT::uint2(pos.x, pos.y);
}

TWT::uint2 TW3D::GetWindowPosition() {
	RECT rect;
	GetWindowRect(hwnd, &rect);
	RECT rect2 = {};
	AdjustWindowRect(&rect2, WS_OVERLAPPEDWINDOW, FALSE);
	return TWT::uint2(rect.left - rect2.left, rect.top - rect2.top);
}

TWT::uint2 TW3D::GetWindowCenterPosition() {
	return GetWindowPosition() + TWT::uint2(width, height) / 2u;
}

TWT::uint2 TW3D::GetCurrentWindowSize() {
	return TWT::uint2(width, height);
}

float TW3D::GetFPS() {
	return 1.0f / delta_time;
}

float TW3D::GetDeltaTime() {
	return delta_time;
}

void TW3D::SetVSync(bool VSync) {
	swapChain->VSync = VSync;
}

void TW3D::SetWindowTitle(const TWT::String& WindowTitle) {
	title = WindowTitle;
	TWT::WString wstrtitle = title.Wide();
	const TWT::wchar* wtitle = wstrtitle.ToWCharArray();
	SetWindowText(hwnd, wtitle);
}

void TW3D::SetRenderer(TW3DRenderer* Renderer) {
	renderer = Renderer;
	renderer->Initialize(resource_manager, swapChain, TWT::uint2(width, height));
	logger->LogInfo("[SetRenderer] Renderer initialized. Resizing renderer to "s + width + "x"s + height);
	renderer->Resize(TWT::uint2(width, height));
	for (int i = 0; i < TW3DSwapChain::BufferCount; i++)
		renderer->InitializeFrame(frames[i]);
	logger->LogInfo("[SetRenderer] Renderer resized."s);
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

void TW3D::SetOnKeyEvent(KeyHandler OnKey) {
	on_key = OnKey;
}

void TW3D::SetOnCharEvent(CharHandler OnChar) {
	on_char = OnChar;
}

TW3DResourceManager* TW3D::GetResourceManager() {
	return resource_manager;
}