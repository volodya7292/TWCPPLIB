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
#include "Raytracing.hlsl.h"

static TW3D::TW3DScene*					scene;
static TW3D::TW3DRenderer*				renderer;
static TW3D::TW3DResourceManager*		resource_manager;

static TWT::UInt		width, height;
static TWT::String		title;
static TWT::Bool		fullscreen, vsync;

static TWT::Bool		running, minimized = false;
static HWND				hwnd;


namespace GlobalRootSignatureParams {
	enum Value {
		OutputViewSlot = 0,
		AccelerationStructureSlot,
		SceneConstantSlot,
		VertexBuffersSlot,
		Count
	};
}
namespace LocalRootSignatureParams {
	enum Value {
		CubeConstantSlot = 0,
		Count
	};
}
struct SceneConstantBuffer {
	TWT::Matrix4f projectionToWorld;
	TWT::Vector4f cameraPosition;
	TWT::Vector4f lightPosition;
	TWT::Vector4f lightAmbientColor;
	TWT::Vector4f lightDiffuseColor;
};
struct CubeConstantBuffer {
	TWT::Vector4f albedo;
};

struct Vertex {
	TWT::Vector3f position;
	TWT::Vector3f normal;
};
typedef UINT16 Index;
union AlignedSceneConstantBuffer {
	SceneConstantBuffer constants;
	uint8_t alignmentPadding[D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT];
};
static AlignedSceneConstantBuffer*  m_mappedConstantData;
static ID3D12Resource*       m_perFrameConstants;
static ID3D12RaytracingFallbackDevice* m_fallbackDevice;
static ID3D12RaytracingFallbackCommandList* m_fallbackCommandList;
static ID3D12RaytracingFallbackStateObject* m_fallbackStateObject;
static WRAPPED_GPU_POINTER m_fallbackTopLevelAccelerationStructurePointer;
static ID3D12RootSignature* m_raytracingGlobalRootSignature;
static ID3D12RootSignature* m_raytracingLocalRootSignature;
static ID3D12DescriptorHeap* m_descriptorHeap;
static UINT m_descriptorsAllocated;
static UINT m_descriptorSize;
static CubeConstantBuffer m_cubeCB;
static SceneConstantBuffer m_sceneCB[TW3D::TW3DSwapChain::BufferCount];
static ID3D12Resource* m_indexBuffer;
static D3D12_CPU_DESCRIPTOR_HANDLE m_indexBuffer_cpuDescriptorHandle;
static D3D12_GPU_DESCRIPTOR_HANDLE m_indexBuffer_gpuDescriptorHandle;
static ID3D12Resource* m_vertexBuffer;
static D3D12_CPU_DESCRIPTOR_HANDLE m_vertexBuffer_cpuDescriptorHandle;
static D3D12_GPU_DESCRIPTOR_HANDLE m_vertexBuffer_gpuDescriptorHandle;
static ID3D12Resource* m_bottomLevelAccelerationStructure;
static ID3D12Resource* m_topLevelAccelerationStructure;
static ID3D12Resource* m_missShaderTable;
static ID3D12Resource* m_hitGroupShaderTable;
static ID3D12Resource* m_rayGenShaderTable;
static ID3D12Resource* m_raytracingOutput;
static D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
static UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;
const wchar_t* c_hitGroupName = L"MyHitGroup";
const wchar_t* c_raygenShaderName = L"MyRaygenShader";
const wchar_t* c_closestHitShaderName = L"MyClosestHitShader";
const wchar_t* c_missShaderName = L"MyMissShader";

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

static TW3D::TW3DFactory*		factory;
static TW3D::TW3DAdapter*		adapter;
static TW3D::TW3DDevice*		device;
static TW3D::TW3DSwapChain*		swapChain;

static TW3D::TW3DCommandQueue* commandQueue;
static TW3D::TW3DGraphicsCommandList* commandList;
static TW3D::TW3DFence* fence[TW3D::TW3DSwapChain::BufferCount];
static TW3D::TW3DPipelineState* pipelineState;
static TW3D::TW3DPipelineState* blitPipelineState;

static TW3D::TW3DResourceRTV* renderTargets[TW3D::TW3DSwapChain::BufferCount];
static TW3D::TW3DResourceDSV* depthStencil;
static TW3D::TW3DResourceRTV *offscreen, *offscreen2;
static TW3D::TW3DResourceSV* texture;
static TW3D::TW3DResourceSV* texture2;

static TW3D::TW3DCube* cube;

void on_resize();

static int frameIndex; // current rtv we are on
static D3D12_VIEWPORT viewport; // area that output from rasterizer will be stretched to.
static D3D12_RECT scissorRect; // the area to draw in. pixels outside that area will not be drawn onto



static int numCubeVertices; // the number of indices to draw the cube

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
	swapChain = new TW3D::TW3DSwapChain(factory, commandQueue, hwnd, width, height, vsync);

	frameIndex = swapChain->GetCurrentBufferIndex();

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++)
		renderTargets[i] = resource_manager->CreateRenderTargetView(swapChain->GetBuffer(i));

	offscreen = resource_manager->CreateRenderTargetView(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, TWT::Vector4f(0, 0, 0, 1));
	offscreen2 = resource_manager->CreateRenderTargetView(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, TWT::Vector4f(0, 0, 0, 1));

	commandList = TW3D::TW3DGraphicsCommandList::CreateDirect(device);
	commandList->Close();

	// create the fences
	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; i++) {
		fence[i] = new TW3D::TW3DFence(device);
	}
	

	TW3D::TW3DRootSignature* rootSignature = new TW3D::TW3DRootSignature(4,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	rootSignature->SetParameterCBV(0, D3D12_SHADER_VISIBILITY_VERTEX, 0);
	rootSignature->SetParameterCBV(1, D3D12_SHADER_VISIBILITY_VERTEX, 1);
	rootSignature->SetParameterSV(2, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignature->SetParameterSV(3, D3D12_SHADER_VISIBILITY_PIXEL, 2);
	rootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->AddSampler(1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->Create(device);

	TW3D::TW3DRootSignature* blitRootSignature = new TW3D::TW3DRootSignature(1,
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	blitRootSignature->SetParameterSV(0, D3D12_SHADER_VISIBILITY_PIXEL, 0);
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
		2);
	pipelineState->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	pipelineState->SetRTVFormat(1, DXGI_FORMAT_R8G8B8A8_UNORM);
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

	TW3DPrimitives::Initialize(resource_manager);
	cube = new TW3D::TW3DCube(resource_manager);

	// Create the depth/stencil buffer
	depthStencil = new TW3D::TW3DResourceDSV(device);
	depthStencil->Create(width, height);

	camera = new TW3D::TW3DPerspectiveCamera(resource_manager, 45, TWT::Vector3f(0.0f, 0.0f, 4.0f), TWT::Vector3f(0, 0, 0));


	texture = resource_manager->CreateTexture2D(L"D:\\òåñò.png");
	texture2 = resource_manager->CreateTexture2D(L"D:\\test2.png");

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
}

void AllocateUploadBuffer(ID3D12Device* pDevice, void *pData, UINT64 datasize, ID3D12Resource **ppResource, const wchar_t* resourceName = nullptr) {
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);
	TWU::SuccessAssert(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName) {
		(*ppResource)->SetName(resourceName);
	}
	void *pMappedData;
	(*ppResource)->Map(0, nullptr, &pMappedData);
	memcpy(pMappedData, pData, datasize);
	(*ppResource)->Unmap(0, nullptr);
}

UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX) {
	auto descriptorHeapCpuBase = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	if (descriptorIndexToUse >= m_descriptorHeap->GetDesc().NumDescriptors) {
		descriptorIndexToUse = m_descriptorsAllocated++;
	}
	*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, m_descriptorSize);
	return descriptorIndexToUse;
}

UINT CreateBufferSRV(ID3D12Resource* buffer, D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptorHandle, D3D12_GPU_DESCRIPTOR_HANDLE* gpuDescriptorHandle, UINT numElements, UINT elementSize) {
	// SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = numElements;
	if (elementSize == 0) {
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		srvDesc.Buffer.StructureByteStride = 0;
	} else {
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.StructureByteStride = elementSize;
	}
	UINT descriptorIndex = AllocateDescriptor(cpuDescriptorHandle);
	device->CreateShaderResourceView(buffer, &srvDesc, *cpuDescriptorHandle);
	*gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, m_descriptorSize);
	return descriptorIndex;
}

void AllocateUAVBuffer(UINT64 bufferSize, ID3D12Resource **ppResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* resourceName = nullptr) {
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	
	device->CreateCommittedResource(&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialResourceState,
		ppResource);
}

inline void ThrowIfFalse(bool value) {
	if (!value) {
		throw std::runtime_error("GOVNO!");
	}
}

WRAPPED_GPU_POINTER CreateFallbackWrappedPointer(ID3D12Resource* resource, UINT bufferNumElements) {
	D3D12_UNORDERED_ACCESS_VIEW_DESC rawBufferUavDesc = {};
	rawBufferUavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	rawBufferUavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
	rawBufferUavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	rawBufferUavDesc.Buffer.NumElements = bufferNumElements;

	D3D12_CPU_DESCRIPTOR_HANDLE bottomLevelDescriptor;

	// Only compute fallback requires a valid descriptor index when creating a wrapped pointer.
	UINT descriptorHeapIndex = 0;
	if (!m_fallbackDevice->UsingRaytracingDriver()) {
		descriptorHeapIndex = AllocateDescriptor(&bottomLevelDescriptor);
		device->Get()->CreateUnorderedAccessView(resource, nullptr, &rawBufferUavDesc, bottomLevelDescriptor);
	}
	return m_fallbackDevice->GetWrappedPointerSimple(descriptorHeapIndex, resource->GetGPUVirtualAddress());
}


void FlushGPU() {
	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		fence[n]->Flush(commandQueue);
}

void BuildAccelerationStructures() {
	// Reset the command list for the acceleration structure construction.
	commandList->Reset();

	D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
	geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDesc.Triangles.IndexBuffer = m_indexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer->GetDesc().Width) / sizeof(Index);
	geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
	geometryDesc.Triangles.Transform3x4 = 0;
	geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer->GetDesc().Width) / sizeof(Vertex);
	geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer->GetGPUVirtualAddress();
	geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

	// Mark the geometry as opaque. 
	// PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
	// Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

	// Get required sizes for an acceleration structure.
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &bottomLevelInputs = bottomLevelBuildDesc.Inputs;
	bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	bottomLevelInputs.Flags = buildFlags;
	bottomLevelInputs.NumDescs = 1;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &geometryDesc;

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &topLevelInputs = topLevelBuildDesc.Inputs;
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = buildFlags;
	topLevelInputs.NumDescs = 1;
	topLevelInputs.pGeometryDescs = nullptr;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
	m_fallbackDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
	ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
	m_fallbackDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
	ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);
	
	ComPtr<ID3D12Resource> scratchResource;
	AllocateUAVBuffer(std::max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes), &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

	// Allocate resources for acceleration structures.
	// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
	// Default heap is OK since the application doesn’t need CPU read/write access to them. 
	// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
	// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
	//  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
	//  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
	
		D3D12_RESOURCE_STATES initialResourceState = m_fallbackDevice->GetAccelerationStructureResourceState();

		AllocateUAVBuffer(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
		AllocateUAVBuffer(topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
	

	// Note on Emulated GPU pointers (AKA Wrapped pointers) requirement in Fallback Layer:
	// The primary point of divergence between the DXR API and the compute-based Fallback layer is the handling of GPU pointers. 
	// DXR fundamentally requires that GPUs be able to dynamically read from arbitrary addresses in GPU memory. 
	// The existing Direct Compute API today is more rigid than DXR and requires apps to explicitly inform the GPU what blocks of memory it will access with SRVs/UAVs.
	// In order to handle the requirements of DXR, the Fallback Layer uses the concept of Emulated GPU pointers, 
	// which requires apps to create views around all memory they will access for raytracing, 
	// but retains the DXR-like flexibility of only needing to bind the top level acceleration structure at DispatchRays.
	//
	// The Fallback Layer interface uses WRAPPED_GPU_POINTER to encapsulate the underlying pointer
	// which will either be an emulated GPU pointer for the compute - based path or a GPU_VIRTUAL_ADDRESS for the DXR path.

	// Create an instance desc for the bottom-level acceleration structure.
	ComPtr<ID3D12Resource> instanceDescs;
		D3D12_RAYTRACING_FALLBACK_INSTANCE_DESC instanceDesc = {};
		instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
		instanceDesc.InstanceMask = 1;
		UINT numBufferElements = static_cast<UINT>(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes) / sizeof(UINT32);
		instanceDesc.AccelerationStructure = CreateFallbackWrappedPointer(m_bottomLevelAccelerationStructure, numBufferElements);
		AllocateUploadBuffer(device->Get(), &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

	// Create a wrapped pointer to the acceleration structure.
		UINT numBufferElements2 = static_cast<UINT>(topLevelPrebuildInfo.ResultDataMaxSizeInBytes) / sizeof(UINT32);
		m_fallbackTopLevelAccelerationStructurePointer = CreateFallbackWrappedPointer(m_topLevelAccelerationStructure, numBufferElements2);
	// Bottom Level Acceleration Structure desc
	{
		bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
		bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
	}

	// Top Level Acceleration Structure desc
	{
		topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
		topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
		topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
	}

	auto BuildAccelerationStructure = [&] (auto* raytracingCommandList) {
		raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
		commandList->ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure));
		raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
	};

	// Build acceleration structure.
		// Set the descriptor heaps to be used during acceleration structure build for the Fallback Layer.
		ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_descriptorHeap };
		m_fallbackCommandList->SetDescriptorHeaps(ARRAYSIZE(pDescriptorHeaps), pDescriptorHeaps);
		BuildAccelerationStructure(m_fallbackCommandList);

	// Kick off acceleration structure construction.
		commandList->Close();
		commandQueue->ExecuteCommandList(commandList);

	// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
		FlushGPU();
}

void CreateConstantBuffers() {
	auto frameCount = swapChain->BufferCount;

	// Create the constant buffer memory and map the CPU and GPU addresses
	const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// Allocate one constant buffer per frame, since it gets updated every frame.
	size_t cbSize = frameCount * sizeof(AlignedSceneConstantBuffer);
	const D3D12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

	device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&constantBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&m_perFrameConstants);

	// Map the constant buffer and cache its heap pointers.
	// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	TWU::SuccessAssert(m_perFrameConstants->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedConstantData)));
}

// Shader record = {{Shader ID}, {RootArguments}}
class ShaderRecord {
public:
	ShaderRecord(void* pShaderIdentifier, UINT shaderIdentifierSize) :
		shaderIdentifier(pShaderIdentifier, shaderIdentifierSize) {
	}

	ShaderRecord(void* pShaderIdentifier, UINT shaderIdentifierSize, void* pLocalRootArguments, UINT localRootArgumentsSize) :
		shaderIdentifier(pShaderIdentifier, shaderIdentifierSize),
		localRootArguments(pLocalRootArguments, localRootArgumentsSize) {
	}

	void CopyTo(void* dest) const {
		uint8_t* byteDest = static_cast<uint8_t*>(dest);
		memcpy(byteDest, shaderIdentifier.ptr, shaderIdentifier.size);
		if (localRootArguments.ptr) {
			memcpy(byteDest + shaderIdentifier.size, localRootArguments.ptr, localRootArguments.size);
		}
	}

	struct PointerWithSize {
		void *ptr;
		UINT size;

		PointerWithSize() : ptr(nullptr), size(0) {}
		PointerWithSize(void* _ptr, UINT _size) : ptr(_ptr), size(_size) {};
	};
	PointerWithSize shaderIdentifier;
	PointerWithSize localRootArguments;
};


class GpuUploadBuffer {
public:
	ComPtr<ID3D12Resource> GetResource() { return m_resource; }

protected:
	ComPtr<ID3D12Resource> m_resource;

	GpuUploadBuffer() {}
	~GpuUploadBuffer() {
		if (m_resource.Get()) {
			m_resource->Unmap(0, nullptr);
		}
	}

	void Allocate(ID3D12Device* device, UINT bufferSize, LPCWSTR resourceName = nullptr) {
		auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
		TWU::SuccessAssert(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_resource)));
		m_resource->SetName(resourceName);
	}

	uint8_t* MapCpuWriteOnly() {
		uint8_t* mappedData;
		// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		TWU::SuccessAssert(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
		return mappedData;
	}
};

// Shader table = {{ ShaderRecord 1}, {ShaderRecord 2}, ...}
class ShaderTable : public GpuUploadBuffer {
	uint8_t* m_mappedShaderRecords;
	UINT m_shaderRecordSize;

	// Debug support
	std::wstring m_name;
	std::vector<ShaderRecord> m_shaderRecords;

	ShaderTable() {}
public:
	inline UINT Align(UINT size, UINT alignment) {
		return (size + (alignment - 1)) & ~(alignment - 1);
	}

	ShaderTable(ID3D12Device* device, UINT numShaderRecords, UINT shaderRecordSize, LPCWSTR resourceName = nullptr)
		: m_name(resourceName) {
		m_shaderRecordSize = Align(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
		m_shaderRecords.reserve(numShaderRecords);
		UINT bufferSize = numShaderRecords * m_shaderRecordSize;
		Allocate(device, bufferSize, resourceName);
		m_mappedShaderRecords = MapCpuWriteOnly();
	}

	void push_back(const ShaderRecord& shaderRecord) {
		ThrowIfFalse(m_shaderRecords.size() < m_shaderRecords.capacity());
		m_shaderRecords.push_back(shaderRecord);
		shaderRecord.CopyTo(m_mappedShaderRecords);
		m_mappedShaderRecords += m_shaderRecordSize;
	}

	UINT GetShaderRecordSize() { return m_shaderRecordSize; }
};

void BuildShaderTables() {
	void* rayGenShaderIdentifier;
	void* missShaderIdentifier;
	void* hitGroupShaderIdentifier;

	auto GetShaderIdentifiers = [&] (auto* stateObjectProperties) {
		rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_raygenShaderName);
		missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
		hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);
	};

	// Get shader identifiers.
	UINT shaderIdentifierSize;
		GetShaderIdentifiers(m_fallbackStateObject);
		shaderIdentifierSize = m_fallbackDevice->GetShaderIdentifierSize();
	

	// Ray gen shader table
	{
		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable rayGenShaderTable(device->Get(), numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
		rayGenShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
		m_rayGenShaderTable = rayGenShaderTable.GetResource().Detach();
	}

	// Miss shader table
	{
		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize;
		ShaderTable missShaderTable(device->Get(), numShaderRecords, shaderRecordSize, L"MissShaderTable");
		missShaderTable.push_back(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
		m_missShaderTable = missShaderTable.GetResource().Detach();
	}

	// Hit group shader table
	{
		struct RootArguments {
			CubeConstantBuffer cb;
		} rootArguments;
		rootArguments.cb = m_cubeCB;

		UINT numShaderRecords = 1;
		UINT shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
		ShaderTable hitGroupShaderTable(device->Get(), numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
		hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
		m_hitGroupShaderTable = hitGroupShaderTable.GetResource().Detach();
	}
}

void CreateRaytracingOutputResource() {
	auto backbufferFormat = swapChain->GetDescription().Format;

	// Create the output resource. The dimensions and format should match the swap-chain.
	auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backbufferFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(
		&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, &m_raytracingOutput);

	D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
	m_raytracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(&uavDescriptorHandle, m_raytracingOutputResourceUAVDescriptorHeapIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
	UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	device->Get()->CreateUnorderedAccessView(m_raytracingOutput, nullptr, &UAVDesc, uavDescriptorHandle);
	m_raytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_raytracingOutputResourceUAVDescriptorHeapIndex, m_descriptorSize);
}

void init_dxr() {
	TWU::SuccessAssert(D3D12CreateRaytracingFallbackDevice(device->Get(), CreateRaytracingFallbackDeviceFlags::ForceComputeFallback, 0, IID_PPV_ARGS(&m_fallbackDevice)));
	m_fallbackDevice->QueryRaytracingCommandList(commandList->Get(), IID_PPV_ARGS(&m_fallbackCommandList));


	CD3DX12_DESCRIPTOR_RANGE ranges[2]; // Perfomance TIP: Order from most frequent to least frequent.
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);  // 1 output texture
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);  // 2 static index and vertex buffers.

	CD3DX12_ROOT_PARAMETER rootParameters[GlobalRootSignatureParams::Count];
	rootParameters[GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &ranges[0]);
	rootParameters[GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
	rootParameters[GlobalRootSignatureParams::SceneConstantSlot].InitAsConstantBufferView(0);
	rootParameters[GlobalRootSignatureParams::VertexBuffersSlot].InitAsDescriptorTable(1, &ranges[1]);
	CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);

	ID3DBlob* blob;
	ID3DBlob* error;
	TWU::SuccessAssert(m_fallbackDevice->D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error));
	TWU::SuccessAssert(m_fallbackDevice->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_raytracingGlobalRootSignature)));

	CD3DX12_ROOT_PARAMETER rootParameters2[LocalRootSignatureParams::Count];
	rootParameters2[LocalRootSignatureParams::CubeConstantSlot].InitAsConstants(SizeOfInUint32(m_cubeCB), 1);
	CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters2), rootParameters2);
	localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
	TWU::SuccessAssert(m_fallbackDevice->D3D12SerializeRootSignature(&localRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error));
	TWU::SuccessAssert(m_fallbackDevice->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_raytracingLocalRootSignature)));



	// Create 7 subobjects that combine into a RTPSO:
	// Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
	// Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
	// This simple sample utilizes default shader association except for local root signature subobject
	// which has an explicit association specified purely for demonstration purposes.
	// 1 - DXIL library
	// 1 - Triangle hit group
	// 1 - Shader config
	// 2 - Local root signature and association
	// 1 - Global root signature
	// 1 - Pipeline config
	CD3D12_STATE_OBJECT_DESC raytracingPipeline { D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


	// DXIL library
	// This contains the shaders and their entrypoints for the state object.
	// Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
	auto lib = raytracingPipeline.CreateSubobject<CD3D12_DXIL_LIBRARY_SUBOBJECT>();
	
	TWT::Int size;
	TWT::Byte* data = TWU::ReadFileBytes("Raytracing.cso", size);
	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;


	//D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void *)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
	lib->SetDXILLibrary(&bytecode);
	// Define which shader exports to surface from the library.
	// If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
	// In this sample, this could be ommited for convenience since the sample uses all shaders in the library. 
	{
		lib->DefineExport(c_raygenShaderName);
		lib->DefineExport(c_closestHitShaderName);
		lib->DefineExport(c_missShaderName);
	}

	// Triangle hit group
	// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
	// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
	auto hitGroup = raytracingPipeline.CreateSubobject<CD3D12_HIT_GROUP_SUBOBJECT>();
	hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
	hitGroup->SetHitGroupExport(c_hitGroupName);
	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	// Shader config
	// Defines the maximum sizes in bytes for the ray payload and attribute structure.
	auto shaderConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
	UINT payloadSize = sizeof(TWT::Vector4f);    // float4 pixelColor
	UINT attributeSize = sizeof(TWT::Vector2f);  // float2 barycentrics
	shaderConfig->Config(payloadSize, attributeSize);

	// Local root signature and shader association
	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
	auto localRootSignature = raytracingPipeline.CreateSubobject<CD3D12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
	localRootSignature->SetRootSignature(m_raytracingLocalRootSignature);
	// Define explicit shader association for the local root signature. 
	{
		auto rootSignatureAssociation = raytracingPipeline.CreateSubobject<CD3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
		rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
		rootSignatureAssociation->AddExport(c_hitGroupName);
	}

	// Global root signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
	auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3D12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
	globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature);

	// Pipeline config
	// Defines the maximum TraceRay() recursion depth.
	auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3D12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
	// PERFOMANCE TIP: Set max recursion depth as low as needed 
	// as drivers may apply optimization strategies for low recursion depths.
	UINT maxRecursionDepth = 1; // ~ primary rays only. 
	pipelineConfig->Config(maxRecursionDepth);

	// Create the state object.
	TWU::SuccessAssert(m_fallbackDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_fallbackStateObject)));



	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	// Allocate a heap for 5 descriptors:
	// 2 - vertex and index buffer SRVs
	// 1 - raytracing output texture SRV
	// 2 - bottom and top level acceleration structure fallback wrapped pointer UAVs
	descriptorHeapDesc.NumDescriptors = 5;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&descriptorHeapDesc, &m_descriptorHeap);

	m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);



	// Cube indices.
	Index indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	// Cube vertices positions and corresponding triangle normals.
	Vertex vertices[] =
	{
		{ TWT::Vector3f(-1.0f, 1.0f, -1.0f), TWT::Vector3f(0.0f, 1.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, 1.0f, -1.0f), TWT::Vector3f(0.0f, 1.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, 1.0f, 1.0f), TWT::Vector3f(0.0f, 1.0f, 0.0f) },
		{ TWT::Vector3f(-1.0f, 1.0f, 1.0f), TWT::Vector3f(0.0f, 1.0f, 0.0f) },

		{ TWT::Vector3f(-1.0f, -1.0f, -1.0f), TWT::Vector3f(0.0f, -1.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, -1.0f, -1.0f), TWT::Vector3f(0.0f, -1.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, -1.0f, 1.0f), TWT::Vector3f(0.0f, -1.0f, 0.0f) },
		{ TWT::Vector3f(-1.0f, -1.0f, 1.0f), TWT::Vector3f(0.0f, -1.0f, 0.0f) },

		{ TWT::Vector3f(-1.0f, -1.0f, 1.0f), TWT::Vector3f(-1.0f, 0.0f, 0.0f) },
		{ TWT::Vector3f(-1.0f, -1.0f, -1.0f), TWT::Vector3f(-1.0f, 0.0f, 0.0f) },
		{ TWT::Vector3f(-1.0f, 1.0f, -1.0f), TWT::Vector3f(-1.0f, 0.0f, 0.0f) },
		{ TWT::Vector3f(-1.0f, 1.0f, 1.0f), TWT::Vector3f(-1.0f, 0.0f, 0.0f) },

		{ TWT::Vector3f(1.0f, -1.0f, 1.0f), TWT::Vector3f(1.0f, 0.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, -1.0f, -1.0f), TWT::Vector3f(1.0f, 0.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, 1.0f, -1.0f), TWT::Vector3f(1.0f, 0.0f, 0.0f) },
		{ TWT::Vector3f(1.0f, 1.0f, 1.0f), TWT::Vector3f(1.0f, 0.0f, 0.0f) },

		{ TWT::Vector3f(-1.0f, -1.0f, -1.0f), TWT::Vector3f(0.0f, 0.0f, -1.0f) },
		{ TWT::Vector3f(1.0f, -1.0f, -1.0f), TWT::Vector3f(0.0f, 0.0f, -1.0f) },
		{ TWT::Vector3f(1.0f, 1.0f, -1.0f), TWT::Vector3f(0.0f, 0.0f, -1.0f) },
		{ TWT::Vector3f(-1.0f, 1.0f, -1.0f), TWT::Vector3f(0.0f, 0.0f, -1.0f) },

		{ TWT::Vector3f(-1.0f, -1.0f, 1.0f), TWT::Vector3f(0.0f, 0.0f, 1.0f) },
		{ TWT::Vector3f(1.0f, -1.0f, 1.0f), TWT::Vector3f(0.0f, 0.0f, 1.0f) },
		{ TWT::Vector3f(1.0f, 1.0f, 1.0f), TWT::Vector3f(0.0f, 0.0f, 1.0f) },
		{ TWT::Vector3f(-1.0f, 1.0f, 1.0f), TWT::Vector3f(0.0f, 0.0f, 1.0f) },
	};

	AllocateUploadBuffer(device->Get(), indices, sizeof(indices), &m_indexBuffer);
	AllocateUploadBuffer(device->Get(), vertices, sizeof(vertices), &m_vertexBuffer);

	// Vertex buffer is passed to the shader along with index buffer as a descriptor table.
	// Vertex buffer descriptor must follow index buffer descriptor in the descriptor heap.
	UINT descriptorIndexIB = CreateBufferSRV(m_indexBuffer, &m_indexBuffer_cpuDescriptorHandle, &m_indexBuffer_gpuDescriptorHandle, sizeof(indices)/4, 0);
	UINT descriptorIndexVB = CreateBufferSRV(m_vertexBuffer, &m_vertexBuffer_cpuDescriptorHandle, &m_vertexBuffer_gpuDescriptorHandle, ARRAYSIZE(vertices), sizeof(vertices[0]));

	BuildAccelerationStructures();
	CreateConstantBuffers();
	BuildShaderTables();
	CreateRaytracingOutputResource();
}

void DoRaytracing() {
	auto DispatchRays = [&] (auto* commandList, auto* stateObject, auto* dispatchDesc) {
		// Since each shader table has only one shader record, the stride is same as the size.
		dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
		dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
		dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;
		dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
		dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
		dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;
		dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
		dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;
		dispatchDesc->Width = width;
		dispatchDesc->Height = height;
		dispatchDesc->Depth = 1;
		commandList->SetPipelineState1(stateObject);
		commandList->DispatchRays(dispatchDesc);
	};
	auto SetCommonPipelineState = [&] (auto* descriptorSetCommandList) {
		descriptorSetCommandList->SetDescriptorHeaps(1, &m_descriptorHeap);
		// Set index and successive vertex buffer decriptor tables
		commandList->Get()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::VertexBuffersSlot, m_indexBuffer_gpuDescriptorHandle);
		commandList->Get()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, m_raytracingOutputResourceUAVGpuDescriptor);
	};

	commandList->Get()->SetComputeRootSignature(m_raytracingGlobalRootSignature);

	// Copy the updated scene constant buffer to GPU.
	memcpy(&m_mappedConstantData[frameIndex].constants, &m_sceneCB[frameIndex], sizeof(m_sceneCB[frameIndex]));
	auto cbGpuAddress = m_perFrameConstants->GetGPUVirtualAddress() + frameIndex * sizeof(m_mappedConstantData[0]);
	commandList->Get()->SetComputeRootConstantBufferView(GlobalRootSignatureParams::SceneConstantSlot, cbGpuAddress);

	// Bind the heaps, acceleration structure and dispatch rays.
	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
	SetCommonPipelineState(m_fallbackCommandList);
	m_fallbackCommandList->SetTopLevelAccelerationStructure(GlobalRootSignatureParams::AccelerationStructureSlot, m_fallbackTopLevelAccelerationStructurePointer);

	DispatchRays(m_fallbackCommandList, m_fallbackStateObject, &dispatchDesc);
}

void CopyRaytracingOutputToBackbuffer() {
	D3D12_RESOURCE_BARRIER preCopyBarriers[2];
	preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex]->Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_raytracingOutput, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->Get()->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

	commandList->Get()->CopyResource(renderTargets[frameIndex]->Get(), m_raytracingOutput);

	D3D12_RESOURCE_BARRIER postCopyBarriers[2];
	postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex]->Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
	postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_raytracingOutput, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	commandList->Get()->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
}

void UpdatePipeline() {

	m_sceneCB[frameIndex].cameraPosition = TWT::Vector4f(0.0f, 2.0f, -5.0f, 1.0f);
	m_sceneCB[frameIndex].projectionToWorld = glm::inverse(camera->GetProjectionMatrix(width, height) * camera->GetViewMatrix());
	m_sceneCB[frameIndex].lightDiffuseColor = TWT::Vector4f(0.5f, 0.0f, 0.0f, 1.0f);
	m_sceneCB[frameIndex].lightAmbientColor = TWT::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
	m_sceneCB[frameIndex].lightPosition = TWT::Vector4f(0.0f, 1.8f, -3.0f, 0.0f);

	m_cubeCB.albedo = TWT::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);



	TWT::Vector3f r = camera->GetRotation();
	r.y += 5;
	camera->SetRotation(r);

	//TWT::Vector4f m_eye( 0.0f, 2.0f, 6.0f, 1.0f);
	//TWT::Vector4f m_at(0.0f, 0.0f, 0.0f, 1.0f);
	//TWT::Vector4f right(1.0f, 0.0f, 0.0f, 0.0f);

	//TWT::Vector4f direction = normalize(m_at - m_eye);

	////XMVECTOR direction = XMVector4Normalize(m_at - m_eye);
	//TWT::Vector3f m_up = glm::normalize(glm::cross(TWT::Vector3f(direction), TWT::Vector3f(right)));

	//// Rotate camera around Y axis.
	//TWT::Matrix4f rotate = glm::rotate(TWT::Matrix4f(1), glm::radians(0.0f), TWT::Vector3f(0, 1, 0));
	//m_eye = m_eye * rotate;
	//m_up = TWT::Vector4f(m_up, 1) * rotate;


	//m_sceneCB[frameIndex].cameraPosition = m_eye;
	//float fovAngleY = 45.0f;
	//TWT::Matrix4f view = glm::lookAt(TWT::Vector3f(m_eye), TWT::Vector3f(m_at), TWT::Vector3f(m_up));
	//TWT::Matrix4f proj = glm::perspectiveFovLH(glm::radians(fovAngleY), (float)width, (float)height, 1.0f, 125.0f);
	//TWT::Matrix4f viewProj = view * proj;

	//m_sceneCB[frameIndex].projectionToWorld = glm::inverse(viewProj);

	//camera->Position -= 0.01f;

	commandList->Reset();

	//commandList->SetPipelineState(pipelineState);

	//commandList->BindResources(resource_manager);

	//commandList->SetRenderTargets({offscreen, offscreen2}, depthStencil);
	//const float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	//commandList->ClearRTV(offscreen);
	//commandList->ClearRTV(offscreen2);
	//commandList->ClearDSVDepth(depthStencil, 1.0f);

	//commandList->BindTexture(2, texture);
	//commandList->BindTexture(3, texture2);

	//commandList->SetViewport(&viewport); // set the viewports
	//commandList->SetScissor(&scissorRect); // set the scissor rects
	//commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	//camera->Use(commandList);


	//commandList->DrawObject(cube, 1);


	//commandList->SetPipelineState(blitPipelineState);

	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//commandList->SetRenderTarget(renderTargets[frameIndex], depthStencil);
	//commandList->ClearRTV(renderTargets[frameIndex]);
	//commandList->ClearDSVDepth(depthStencil, 1.0f);
	//commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//commandList->BindRTVTexture(0, offscreen);

	//commandList->Draw(4);

	//commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	DoRaytracing();
	CopyRaytracingOutputToBackbuffer();

	commandList->Close();
}

void update() {
	cube->transform.AdjustRotation(TWT::Vector3f(0.01f));
	//camera->UpdateConstantBuffer(width, height);

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
	offscreen2->Release();

	depthStencil->Release();

	swapChain->Resize(width, height);

	frameIndex = swapChain->GetCurrentBufferIndex();

	for (UINT n = 0; n < TW3D::TW3DSwapChain::BufferCount; n++)
		renderTargets[n]->Create(swapChain->GetBuffer(n));
	offscreen->Create(width, height);
	offscreen2->Create(width, height);

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

	for (int i = 0; i < TW3D::TW3DSwapChain::BufferCount; ++i) {
		delete renderTargets[i];
		delete fence[i];
	};

	delete offscreen;
	delete offscreen2;

	delete pipelineState;
	delete blitPipelineState;

	delete depthStencil;
	delete texture;
	delete texture2;
	delete cube;

	TWU::DXSafeRelease(m_fallbackDevice);
	TWU::DXSafeRelease(m_fallbackCommandList);
	TWU::DXSafeRelease(m_fallbackStateObject);
	TWU::DXSafeRelease(m_raytracingGlobalRootSignature);
	TWU::DXSafeRelease(m_raytracingLocalRootSignature);
	TWU::DXSafeRelease(m_indexBuffer);
	TWU::DXSafeRelease(m_vertexBuffer);
	TWU::DXSafeRelease(m_descriptorHeap);
	TWU::DXSafeRelease(m_topLevelAccelerationStructure);
	TWU::DXSafeRelease(m_bottomLevelAccelerationStructure);
	TWU::DXSafeRelease(m_perFrameConstants);
	TWU::DXSafeRelease(m_raytracingOutput);
	/*TWU::DXSafeRelease(m_missShaderTable);
	TWU::DXSafeRelease(m_hitGroupShaderTable);
	TWU::DXSafeRelease(m_rayGenShaderTable);
	TWU::DXSafeRelease(m_raytracingOutput);*/

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
	init_dxr();
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
