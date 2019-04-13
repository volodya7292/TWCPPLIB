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

static TW3D::TW3DScene*					scene;
static TW3D::TW3DRenderer*				renderer;
static TW3D::TW3DResourceManager*		resource_manager;

static TWT::UInt		width, height;
static TWT::String		title;
static TWT::Bool		fullscreen, vsync;

static TWT::Bool		running, minimized = false;
static HWND				hwnd;

static TW3D::TW3DFactory*		factory;
static TW3D::TW3DAdapter*		adapter;
static TW3D::TW3DDevice*		device;
static TW3D::TW3DSwapChain*		swapChain;

static TW3D::TW3DCommandQueue* commandQueue;
static TW3D::TW3DCommandQueue* computeCommandQueue;
static TW3D::TW3DGraphicsCommandList* commandList;
static TW3D::TW3DGraphicsCommandList* computeCommandList;
static TW3D::TW3DFence* fence[TW3D::TW3DSwapChain::BufferCount];
static TW3D::TW3DGraphicsPipelineState* pipelineState;
static TW3D::TW3DGraphicsPipelineState* blitPipelineState;
static TW3D::TW3DComputePipelineState* computePipelineState;

static TW3D::TW3DResourceRTV* renderTargets[TW3D::TW3DSwapChain::BufferCount];
static TW3D::TW3DResourceDSV* depthStencil;
static TW3D::TW3DResourceRTV* offscreen, *offscreen2;
static TW3D::TW3DResourceUAV* uavTex;
static TW3D::TW3DResourceSR* texture;
static TW3D::TW3DResourceSR* texture2;

static TW3D::TW3DCube* cube;

void on_resize();

static int frameIndex; // current rtv we are on
static D3D12_VIEWPORT viewport; // area that output from rasterizer will be stretched to.
static D3D12_RECT scissorRect; // the area to draw in. pixels outside that area will not be drawn onto


static TW3D::TW3DPerspectiveCamera* camera;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

	if (!hwnd)
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
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
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
			debugController->EnableDebugLayer();
		} else {
			OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
		}

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

	commandQueue = TW3D::TW3DCommandQueue::CreateDirect(device);
	computeCommandQueue = TW3D::TW3DCommandQueue::CreateCompute(device);
	swapChain = new TW3D::TW3DSwapChain(factory, commandQueue, hwnd, width, height, vsync);

	frameIndex = swapChain->GetCurrentBufferIndex();

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++)
		renderTargets[i] = resource_manager->CreateRenderTargetView(swapChain->GetBuffer(i));

	offscreen = resource_manager->CreateRenderTargetView(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, TWT::Vector4f(0, 0, 0, 1));
	offscreen2 = resource_manager->CreateRenderTargetView(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, TWT::Vector4f(0, 0, 0, 1));


	commandList = TW3D::TW3DGraphicsCommandList::CreateDirect(device);
	commandList->Close();

	computeCommandList = TW3D::TW3DGraphicsCommandList::CreateCompute(device);
	computeCommandList->Close();

	// create the fences
	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++) {
		fence[i] = new TW3D::TW3DFence(device);
	}
	

	TW3D::TW3DRootSignature* rootSignature = new TW3D::TW3DRootSignature(
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	rootSignature->SetParameterCBV(0, D3D12_SHADER_VISIBILITY_VERTEX, 0);
	rootSignature->SetParameterCBV(1, D3D12_SHADER_VISIBILITY_VERTEX, 1);
	rootSignature->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_PIXEL, 0, 2);
	rootSignature->SetParameterUAV(3, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->AddSampler(1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->Create(device);

	TW3D::TW3DRootSignature* blitRootSignature = new TW3D::TW3DRootSignature(
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	blitRootSignature->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	blitRootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	blitRootSignature->Create(device);


	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(2);
	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;
	rastDesc.FrontCounterClockwise = TRUE;
	rastDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rastDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rastDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rastDesc.DepthClipEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;
	rastDesc.ForcedSampleCount = 0;
	rastDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthDesc.StencilEnable = FALSE;
	depthDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	depthDesc.FrontFace = defaultStencilOp;
	depthDesc.BackFace = defaultStencilOp;

	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	
	pipelineState = new TW3D::TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		swapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		rootSignature,
		2);
	pipelineState->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	pipelineState->SetRTVFormat(1, DXGI_FORMAT_R8G8B8A8_UNORM);
	pipelineState->SetVertexShader("VertexShader.cso");
	pipelineState->SetPixelShader("PixelShader.cso");
	pipelineState->SetInputLayout(inputLayout);
	pipelineState->Create(device);


	rastDesc.CullMode = D3D12_CULL_MODE_NONE;
	depthDesc.DepthEnable = FALSE;

	blitPipelineState = new TW3D::TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		swapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		blitRootSignature,
		1);
	blitPipelineState->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	blitPipelineState->SetVertexShader("VertexOffscreenBlit.cso");
	blitPipelineState->SetPixelShader("PixelOffscreenBlit.cso");
	blitPipelineState->Create(device);


	TW3D::TW3DRootSignature* computeRootSignature = new TW3D::TW3DRootSignature();
	computeRootSignature->SetParameterUAV(0, D3D12_SHADER_VISIBILITY_ALL, 0);
	//computeRootSignature->SetParameterSV(0, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	//computeRootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	computeRootSignature->Create(device);

	computePipelineState = new TW3D::TW3DComputePipelineState(computeRootSignature);
	computePipelineState->SetShader("RayTrace.cso");
	computePipelineState->Create(device);


	TW3DPrimitives::Initialize(resource_manager);
	cube = new TW3D::TW3DCube(resource_manager);

	// Create the depth/stencil buffer
	depthStencil = resource_manager->CreateDepthStencilView(width, height);

	camera = new TW3D::TW3DPerspectiveCamera(resource_manager, width, height, 45, TWT::Vector3f(0.0f, 0.0f, 3.0f), TWT::Vector3f(0, 0, 0));


	texture = resource_manager->CreateTexture2D(L"D:\\тест.png");
	texture2 = resource_manager->CreateTexture2D(L"D:\\test2.png");

	uavTex = resource_manager->CreateUnorderedAccessView(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	fence[frameIndex]->Flush(commandQueue);
	fence[frameIndex]->Flush(computeCommandQueue);


	// Fill out the Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = width;
	scissorRect.bottom = height;
}

void FlushGPU() {
	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		fence[n]->Flush(computeCommandQueue);
	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		fence[n]->Flush(commandQueue);
}

void UpdatePipeline() {
	computeCommandList->Reset();
	computeCommandList->SetPipelineState(computePipelineState);
	computeCommandList->BindResources(resource_manager);
	computeCommandList->BindUAVTexture(0, uavTex);
	computeCommandList->Dispatch(width, height);
	computeCommandList->Close();




	commandList->Reset();

	commandList->SetPipelineState(pipelineState);

	commandList->BindResources(resource_manager);

	commandList->SetRenderTargets({offscreen, offscreen2}, depthStencil);
	const float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	commandList->ClearRTV(offscreen);
	commandList->ClearRTV(offscreen2);
	commandList->ClearDSVDepth(depthStencil, 1.0f);

	commandList->BindTexture(2, texture);
	commandList->BindUAVTexture(3, uavTex);
	//commandList->BindTexture(3, texture2);

	commandList->SetViewport(&viewport); // set the viewports
	commandList->SetScissor(&scissorRect); // set the scissor rects
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	camera->Use(commandList);


	commandList->DrawObject(cube, 1);


	commandList->SetPipelineState(blitPipelineState);

	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	commandList->SetRenderTarget(renderTargets[frameIndex], depthStencil);
	commandList->ClearRTV(renderTargets[frameIndex]);
	commandList->ClearDSVDepth(depthStencil, 1.0f);
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	commandList->BindRTVTexture(0, offscreen);

	commandList->Draw(4);

	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	commandList->Close();
}

void update() {
	cube->transform.AdjustRotation(TWT::Vector3f(0.01f));
	camera->UpdateConstantBuffer();
}

void render() {
	fence[frameIndex]->Flush(computeCommandQueue);
	fence[frameIndex]->Flush(commandQueue);
	frameIndex = swapChain->GetCurrentBufferIndex();

	UpdatePipeline(); // update the pipeline by sending commands to the commandqueue

	computeCommandQueue->ExecuteCommandList(computeCommandList);
	commandQueue->ExecuteCommandList(commandList);

	swapChain->Present();
}

void on_resize() {
	FlushGPU();

	RECT clientRect = {};

	GetClientRect(hwnd, &clientRect);
	width = std::max(clientRect.right - clientRect.left, 1L);
	height = std::max(clientRect.bottom - clientRect.top, 1L);

	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);

	scissorRect.right = width;
	scissorRect.bottom = height;

	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		renderTargets[n]->Release();

	offscreen->Release();
	offscreen2->Release();
	uavTex->Release();

	depthStencil->Release();

	swapChain->Resize(width, height);

	frameIndex = swapChain->GetCurrentBufferIndex();

	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		renderTargets[n]->Create(swapChain->GetBuffer(n));
	offscreen->Create(width, height);
	offscreen2->Create(width, height);
	uavTex->CreateTexture2D(width, height);

	depthStencil->Create(width, height);
}

void main_loop() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	running = true;

	while (running) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			// run game code
			update(); // update the game logic
			if (!minimized)
				render(); // execute the command queue (rendering the scene is the result of the gpu executing the command lists)
		}
	}

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
	delete camera;

	delete commandQueue;
	delete commandList;
	delete computeCommandQueue;
	delete computeCommandList;

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; ++i) {
		delete renderTargets[i];
		delete fence[i];
	};

	delete offscreen;
	delete offscreen2;

	delete pipelineState;
	delete blitPipelineState;
	delete computePipelineState;

	delete depthStencil;
	delete texture;
	delete texture2;
	delete uavTex;
	delete cube;

	TW3DPrimitives::Cleanup();

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
#endif // _DEBUG
}

void TW3D::Initialize(const InitializeInfo& info) {
	width = info.width;
	height = info.height;
	title = info.title;
	fullscreen = info.fullscreen;
	vsync = info.vsync;

	init_window();
	init_dx12();
}

void TW3D::Start() {
	main_loop();
	cleanup();
}

void TW3D::SetScene(TW3DScene* Scene) {
	scene = Scene;
}

void TW3D::SetRenderer(TW3DRenderer* Renderer) {
	renderer = Renderer;
}

TW3D::TW3DResourceManager* GetResourceManager() {
	return resource_manager;
}
