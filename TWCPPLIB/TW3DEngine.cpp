#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DDevice.h"
#include "TW3DSwapChain.h"
#include "TW3DFence.h"
#include "TW3DResourceDSV.h"
#include "TW3DResourceSV.h"
#include "TW3DResourceRTV.h"
#include "TW3DResourceVB.h"
#include "TW3DTempGCL.h"
#include "TW3DPipelineState.h"
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
static TW3D::TW3DGraphicsCommandList* commandList;
static TW3D::TW3DDescriptorHeap* mainDescriptorHeap;
static TW3D::TW3DDescriptorHeap* rtvDescriptorHeap;
static TW3D::TW3DFence* fence[TW3D::TW3DSwapChain::BufferCount];
static TW3D::TW3DTempGCL* tempGCL;
static TW3D::TW3DPipelineState* pipelineState;
static TW3D::TW3DPipelineState* blitPipelineState;
//static TW3D::TW3DRootSignature* rootSignature;
//static TW3D::TW3DRootSignature* blitRootSignature;
//static TW3D::TW3DResourceCB* constantBuffer1, *constantBuffer2;
//static TW3D::TW3DResourceCB* cameraCB;

static TW3D::TW3DResourceRTV* renderTargets[TW3D::TW3DSwapChain::BufferCount];
static TW3D::TW3DResourceDSV* depthStencil;
static TW3D::TW3DResourceRTV* offscreen;
static TW3D::TW3DResourceSV* texture;
static TW3D::TW3DResourceSV* texture2;

//static TW3D::TW3DResourceVB* vertexBuffer;
static TW3D::TW3DCube* cube;

void on_resize();

static int frameIndex; // current rtv we are on
static D3D12_VIEWPORT viewport; // area that output from rasterizer will be stretched to.
static D3D12_RECT scissorRect; // the area to draw in. pixels outside that area will not be drawn onto



static int numCubeVertices; // the number of indices to draw the cube

static TW3D::TW3DPerspectiveCamera* camera;

//TWT::Matrix4f cameraProjMat; // this will store our projection matrix
//TWT::Matrix4f cameraViewMat; // this will store our view matrix

//TWT::Vector4f cameraPosition; // this is our cameras position vector
//TWT::Vector4f cameraTarget; // a vector describing the point in space our camera is looking at
//TWT::Vector4f cameraUp; // the worlds up vector

TWT::Matrix4f cube1WorldMat; // our first cubes world matrix (transformation matrix)
TWT::Matrix4f cube1RotMat; // this will keep track of our rotation for the first cube
TWT::Vector4f cube1Position; // our first cubes position in space

TWT::Matrix4f cube2WorldMat; // our first cubes world matrix (transformation matrix)
TWT::Matrix4f cube2RotMat; // this will keep track of our rotation for the second cube
TWT::Vector4f cube2PositionOffset; // our second cube will rotate around the first cube, so this is the position offset from the first cube


// this is the structure of our constant buffer.
//struct ConstantBufferPerObject {
//	TWT::Matrix4f wvpMat;
//	//TWT::Matrix4f govno[256];
//};

//ConstantBufferPerObject cbPerObject; // this is the constant buffer data we will send to the gpu 
										// (which will be placed in the resource we created above)

struct Vertex {
	Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), texCoord(u, v) {}
	TWT::Vector3f pos;
	TWT::Vector2f texCoord;
};

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
	swapChain = new TW3D::TW3DSwapChain(factory, commandQueue, hwnd, width, height, vsync);

	frameIndex = swapChain->GetCurrentBufferIndex();

	rtvDescriptorHeap = TW3D::TW3DDescriptorHeap::CreateForRTV(device, TW3D::TW3DSwapChain::BufferCount + 1);
	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++) {
		renderTargets[i] = new TW3D::TW3DResourceRTV(device, rtvDescriptorHeap, i);
		renderTargets[i]->Create(swapChain->GetBuffer(i));
	}

	mainDescriptorHeap = TW3D::TW3DDescriptorHeap::CreateForSR(device, 3);

	offscreen = new TW3D::TW3DResourceRTV(device, rtvDescriptorHeap, TW3D::TW3DSwapChain::BufferCount, mainDescriptorHeap, 2, DXGI_FORMAT_R8G8B8A8_UNORM, TWT::Vector4f(0, 0, 0, 1));
	offscreen->Create(width, height);


	tempGCL = new TW3D::TW3DTempGCL(device);


	commandList = TW3D::TW3DGraphicsCommandList::CreateDirect(device);

	// create the fences
	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++) {
		fence[i] = new TW3D::TW3DFence(device);
	}
	
	TWT::Vector<D3D12_DESCRIPTOR_RANGE> ranges(2);
	ranges[0] = TWU::DXDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	ranges[1] = TWU::DXDescriptorRange(2, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	TW3D::TW3DRootSignature* rootSignature = new TW3D::TW3DRootSignature(3,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	rootSignature->SetParameter(0, TW3D::TW3DRootParameter::CreateCBV(0, D3D12_SHADER_VISIBILITY_VERTEX));
	rootSignature->SetParameter(1, TW3D::TW3DRootParameter::CreateCBV(1, D3D12_SHADER_VISIBILITY_VERTEX));
	rootSignature->SetParameter(2, TW3D::TW3DRootParameter(D3D12_SHADER_VISIBILITY_PIXEL, ranges, 0));
	rootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->AddSampler(1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->Create(device);


	TWT::Vector<D3D12_DESCRIPTOR_RANGE> blitranges(1);
	blitranges[0] = TWU::DXDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	TW3D::TW3DRootSignature* blitRootSignature = new TW3D::TW3DRootSignature(1,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	blitRootSignature->SetParameter(0, TW3D::TW3DRootParameter(D3D12_SHADER_VISIBILITY_PIXEL, blitranges, 2));
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

	pipelineState = new TW3D::TW3DPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		swapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		rootSignature,
		1);
	pipelineState->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	pipelineState->SetVertexShader("VertexShader.cso");
	pipelineState->SetPixelShader("PixelShader.cso");
	pipelineState->SetInputLayout(inputLayout);
	pipelineState->Create(device);


	rastDesc.CullMode = D3D12_CULL_MODE_NONE;
	depthDesc.DepthEnable = FALSE;

	blitPipelineState = new TW3D::TW3DPipelineState(
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

	// a cube
	Vertex vList[] = {
		// front face
		{ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{  0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

		//// right side face
		{  0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{  0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f },

		// left side face
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

		// back face
		{  0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f },

		// top face
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f },

		// bottom face
		{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
	};

	int vBufferSize = sizeof(vList);
	numCubeVertices = vBufferSize / sizeof(Vertex);

	//vertexBuffer = new TW3D::TW3DResourceVB(device, vBufferSize, sizeof(Vertex), tempGCL);
	//vertexBuffer->UpdateData(reinterpret_cast<BYTE*>(vList), vBufferSize);

	commandList->Close();

	TW3DPrimitives::Initialize(resource_manager);
	cube = new TW3D::TW3DCube(resource_manager);

	// Create the depth/stencil buffer
	depthStencil = new TW3D::TW3DResourceDSV(device);
	depthStencil->Create(width, height);

	camera = new TW3D::TW3DPerspectiveCamera(resource_manager, 45, TWT::Vector3f(0.0f, 0.0f, 4.0f), TWT::Vector3f(0, 0, 0));

	//constantBuffer2 = new TW3D::TW3DResourceCB(device, sizeof(cbPerObject), 1);

	texture = TW3D::TW3DResourceSV::Create2D(device, mainDescriptorHeap, L"D:\\тест.png", tempGCL, 0);
	texture2 = TW3D::TW3DResourceSV::Create2D(device, mainDescriptorHeap, L"D:\\test2.png", tempGCL, 1);

	fence[frameIndex]->Flush(commandQueue);


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

	// build projection and view matrix
	//TWT::Matrix4f tmpMat = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
	//cameraProjMat = tmpMat;

	// set starting camera state
	//cameraPosition = TWT::Vector4f(0.0f, 2.0f, -4.0f, 0.0f);
	//cameraTarget = TWT::Vector4f(0.0f, 0.0f, 0.0f, 0.0f);
	//cameraUp = TWT::Vector4f(0.0f, 1.0f, 0.0f, 0.0f);

	// build view matrix
	//TWT::Vector3f cPos = cameraPosition;
	//TWT::Vector3f cTarg = cameraTarget;
	//TWT::Vector3f cUp = cameraUp;
	//tmpMat = glm::lookAt(cPos, cTarg, cUp);
	//cameraViewMat = tmpMat;

	// set starting cubes position
	// first cube
	cube1Position = TWT::Vector4f(0.0f, 0.0f, 0.0f, 0.0f); // set cube 1's position
	TWT::Vector3f posVec = cube1Position;

	TWT::Matrix4f tmpMat = glm::translate(TWT::Matrix4f(1), TWT::Vector3f(posVec));
	cube1RotMat = TWT::Matrix4f(1);
	cube1WorldMat = tmpMat;

	// second cube
	cube2PositionOffset = TWT::Vector4f(1.5f, 0.0f, 0.0f, 0.0f);
	posVec = cube2PositionOffset + cube1Position;
													// we are rotating around cube1 here, so add cube2's position to cube1

	tmpMat = glm::translate(TWT::Matrix4f(1), TWT::Vector3f(posVec));
	cube2RotMat = TWT::Matrix4f(1);
	cube2WorldMat = tmpMat;
}

void FlushGPU() {
	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		fence[n]->Flush(commandQueue);
}

void UpdatePipeline() {
	commandList->Reset();

	commandList->SetPipelineState(pipelineState);

	commandList->SetRenderTarget(offscreen, depthStencil);
	const float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	commandList->ClearRTV(offscreen);
	commandList->ClearDSVDepth(depthStencil, 1.0f);

	//commandList->SetGraphicsRootSignature(rootSignature); // set the root signature
	commandList->SetDescriptorHeap(mainDescriptorHeap);

	commandList->SetGraphicsRootDescriptorTable(2, mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	commandList->SetViewport(&viewport); // set the viewports
	commandList->SetScissor(&scissorRect); // set the scissor rects
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	//commandList->SetVertexBuffer(0, vertexBuffer); // set the vertex buffer (using the vertex buffer view)

	camera->Use(commandList);
	//commandList->SetGraphicsRootCBV(0, cameraCB, 0);

	cube->RecordDraw(commandList, 1);
	//commandList->Draw(numCubeVertices);

	//commandList->SetGraphicsRootCBV(0, constantBuffer2, 0);
	//cube->RecordDraw(commandList);
	//commandList->Draw(numCubeVertices);



	commandList->SetPipelineState(blitPipelineState);

	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	commandList->SetRenderTarget(renderTargets[frameIndex], depthStencil);
	commandList->ClearRTV(renderTargets[frameIndex]);
	commandList->ClearDSVDepth(depthStencil, 1.0f);
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//commandList->SetGraphicsRootSignature(blitRootSignature); // set the root signature
	commandList->SetGraphicsRootDescriptorTable(0, mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	commandList->Draw(4);

	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	commandList->Close();
}

void update() {
	// update app logic, such as moving the camera or figuring out what objects are in view

	// create rotation matrices
	TWT::Matrix4f rotXMat = glm::rotate(TWT::Matrix4f(1), 0.01f, TWT::Vector3f(1, 0, 0));
	TWT::Matrix4f rotYMat = glm::rotate(TWT::Matrix4f(1), 0.02f, TWT::Vector3f(0, 1, 0));
	TWT::Matrix4f rotZMat = glm::rotate(TWT::Matrix4f(1), 0.03f, TWT::Vector3f(0, 0, 1));

	// add rotation to cube1's rotation matrix and store it
	TWT::Matrix4f rotMat = cube1RotMat * rotXMat * rotYMat * rotZMat;
	cube1RotMat = rotMat;

	// create translation matrix for cube 1 from cube 1's position vector
	TWT::Matrix4f translationMat = translate(TWT::Matrix4f(1), TWT::Vector3f(cube1Position));

	// create cube1's world matrix by first rotating the cube, then positioning the rotated cube
	TWT::Matrix4f worldMat = rotMat * translationMat;

	// store cube1's world matrix
	cube1WorldMat = worldMat;

	// update constant buffer for cube1
	// create the wvp matrix and store in constant buffer
	//TWT::Matrix4f viewMat = cameraViewMat;
	//TWT::Matrix4f projMat = cameraProjMat;
	//TWT::Matrix4f wvpMat = camera.GetViewProjectionMatrix(width, height) * cube1WorldMat;
	//cbPerObject.wvpMat = wvpMat;
	
	glm::vec3 d = normalize(glm::vec3(5));
	// copy our ConstantBuffer instance to the mapped constant buffer resource
	//constantBuffer1->Update(&cbPerObject, 0);
	//memcpy(cbvGPUAddress[frameIndex], &cbPerObject, sizeof(cbPerObject));

	// now do cube2's world matrix
	// create rotation matrices for cube2
	rotXMat = glm::rotate(TWT::Matrix4f(1), 0.03f, TWT::Vector3f(1, 0, 0));
	rotYMat = glm::rotate(TWT::Matrix4f(1), 0.02f, TWT::Vector3f(0, 1, 0));
	rotZMat = glm::rotate(TWT::Matrix4f(1), 0.01f, TWT::Vector3f(0, 0, 1));


	// add rotation to cube2's rotation matrix and store it
	rotMat = rotZMat * (cube2RotMat * (rotXMat * rotYMat));
	cube2RotMat = rotMat;

	// create translation matrix for cube 2 to offset it from cube 1 (its position relative to cube1
	TWT::Matrix4f translationOffsetMat = glm::translate(TWT::Matrix4f(1), TWT::Vector3f(cube2PositionOffset));

	// we want cube 2 to be half the size of cube 1, so we scale it by .5 in all dimensions
	TWT::Matrix4f scaleMat = glm::scale(TWT::Matrix4f(1), TWT::Vector3f(0.5f));

	// reuse worldMat. 
	// first we scale cube2. scaling happens relative to point 0,0,0, so you will almost always want to scale first
	// then we translate it. 
	// then we rotate it. rotation always rotates around point 0,0,0
	// finally we move it to cube 1's position, which will cause it to rotate around cube 1
	worldMat = scaleMat * translationOffsetMat * rotMat * translationMat;
	
	//wvpMat = camera.GetViewProjectionMatrix(width, height) * cube2WorldMat; // create wvp matrix

	//cbPerObject.wvpMat = wvpMat; // store transposed wvp matrix in constant buffer

	// copy our ConstantBuffer instance to the mapped constant buffer resource
	//constantBuffer2->Update(&cbPerObject, 0);
	//memcpy(cbvGPUAddress[frameIndex] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject));

	// store cube2's world matrix
	cube2WorldMat = worldMat;

	cube->transform.AdjustRotation(TWT::Vector3f(0.01));
	camera->UpdateConstantBuffer(width, height);
	/*double t = TWU::GetTime();
	for (int i = 0; i < 10000; i++) {
		constantBuffer1->Update(&cbPerObject, 0);
		constantBuffer2->Update(&cbPerObject, 0);
	}
	double t2 = TWU::GetTime();
	TWU::CPrintln(t2 - t);*/

}

void render() {
	fence[frameIndex]->Flush(commandQueue);
	frameIndex = swapChain->GetCurrentBufferIndex();

	UpdatePipeline(); // update the pipeline by sending commands to the commandqueue

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

	depthStencil->Release();

	swapChain->Resize(width, height);

	frameIndex = swapChain->GetCurrentBufferIndex();

	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		renderTargets[n]->Create(swapChain->GetBuffer(n));
	offscreen->Create(width, height);

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

	delete tempGCL;
	delete resource_manager;
	delete camera;

	delete commandQueue;
	delete rtvDescriptorHeap;
	delete commandList;

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; ++i) {
		delete renderTargets[i];
		delete fence[i];
	};

	delete offscreen;

	//delete constantBuffer1;
	delete mainDescriptorHeap;
	delete pipelineState;
	delete blitPipelineState;
	//delete rootSignature;
	//delete blitRootSignature;
	//delete vertexBuffer;

	delete depthStencil;
	delete texture;
	delete texture2;
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
