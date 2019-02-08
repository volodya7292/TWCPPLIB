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

using namespace DirectX;

static TWT::UInt		width, height;
static TWT::String		title;
static TWT::Bool		fullscreen, vsync;

static TWT::Bool		running, minimized = false;
static HWND				hwnd;

static const int frameBufferCount = 3; // number of buffers we want, 2 for double buffering, 3 for tripple buffering

static TW3D::TW3DFactory* factory;
static TW3D::TW3DAdapter* adapter;
static TW3D::TW3DDevice* device;
static TW3D::TW3DSwapChain* swapChain;

static TW3D::TW3DCommandQueue* commandQueue;
static TW3D::TW3DGraphicsCommandList* commandList;
static TW3D::TW3DDescriptorHeap* mainDescriptorHeap;
static TW3D::TW3DDescriptorHeap* rtvDescriptorHeap;
static TW3D::TW3DFence* fence[frameBufferCount];
static TW3D::TW3DTempGCL* tempGCL;
static TW3D::TW3DPipelineState* pipelineState;

static TW3D::TW3DResourceRTV* renderTargets[frameBufferCount];
static TW3D::TW3DResourceDSV* depthStencil;
static TW3D::TW3DResourceSV* texture;

static TW3D::TW3DResourceVB* vertexBuffer;
//static TW3D::TW3DResource* vertexBuffer;
static TW3D::TW3DResource* indexBuffer;
//static TW3D::TW3DResource* vBufferUploadHeap;
static TW3D::TW3DResource* iBufferUploadHeap;
static TW3D::TW3DResource* constantBufferUploadHeaps[frameBufferCount];

void on_resize();

static int frameIndex; // current rtv we are on
static ID3D12RootSignature* rootSignature;
static D3D12_VIEWPORT viewport; // area that output from rasterizer will be stretched to.

static D3D12_RECT scissorRect; // the area to draw in. pixels outside that area will not be drawn onto

static D3D12_VERTEX_BUFFER_VIEW vertexBufferView; // a structure containing a pointer to the vertex data in gpu memory
										   // the total size of the buffer, and the size of each element (vertex)

static D3D12_INDEX_BUFFER_VIEW indexBufferView; // a structure holding information about the index buffer
												 //as we have allocators (more if we want to know when the gpu is finished with an asset)



static int numCubeIndices; // the number of indices to draw the cube
static UINT8* cbvGPUAddress[frameBufferCount]; // this is a pointer to each of the constant buffer resource heaps

XMFLOAT4X4 cameraProjMat; // this will store our projection matrix
XMFLOAT4X4 cameraViewMat; // this will store our view matrix

XMFLOAT4 cameraPosition; // this is our cameras position vector
XMFLOAT4 cameraTarget; // a vector describing the point in space our camera is looking at
XMFLOAT4 cameraUp; // the worlds up vector

XMFLOAT4X4 cube1WorldMat; // our first cubes world matrix (transformation matrix)
XMFLOAT4X4 cube1RotMat; // this will keep track of our rotation for the first cube
XMFLOAT4 cube1Position; // our first cubes position in space

XMFLOAT4X4 cube2WorldMat; // our first cubes world matrix (transformation matrix)
XMFLOAT4X4 cube2RotMat; // this will keep track of our rotation for the second cube
XMFLOAT4 cube2PositionOffset; // our second cube will rotate around the first cube, so this is the position offset from the first cube


// this is the structure of our constant buffer.
struct ConstantBufferPerObject {
	XMFLOAT4X4 wvpMat;
};

// Constant buffers must be 256-byte aligned which has to do with constant reads on the GPU.
// We are only able to read at 256 byte intervals from the start of a resource heap, so we will
// make sure that we add padding between the two constant buffers in the heap (one for cube1 and one for cube2)
// Another way to do this would be to add a float array in the constant buffer structure for padding. In this case
// we would need to add a float padding[50]; after the wvpMat variable. This would align our structure to 256 bytes (4 bytes per float)
// The reason i didn't go with this way, was because there would actually be wasted cpu cycles when memcpy our constant
// buffer data to the gpu virtual address. currently we memcpy the size of our structure, which is 16 bytes here, but if we
// were to add the padding array, we would memcpy 64 bytes if we memcpy the size of our structure, which is 50 wasted bytes
// being copied.
int ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBufferPerObject) + 255) & ~255;

ConstantBufferPerObject cbPerObject; // this is the constant buffer data we will send to the gpu 
										// (which will be placed in the resource we created above)

struct Vertex {
	Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), texCoord(u, v) {}
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
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

	if (!hwnd) {
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
	}

	if (fullscreen) {
		SetWindowLong(hwnd, GWL_STYLE, 0);
	}

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

void init_dx12() {
	TWT::UInt dxgi_factory_flags = 0;

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
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

	commandQueue = TW3D::TW3DCommandQueue::CreateDirect(device);
	swapChain = new TW3D::TW3DSwapChain(factory, commandQueue, hwnd, width, height, vsync);

	frameIndex = swapChain->GetCurrentBufferIndex();

	rtvDescriptorHeap = TW3D::TW3DDescriptorHeap::CreateForRTV(device, frameBufferCount);
	for (int i = 0; i < frameBufferCount; i++) {
		renderTargets[i] = new TW3D::TW3DResourceRTV(device, rtvDescriptorHeap);
		renderTargets[i]->Create(swapChain->GetBuffer(i), i);
	}


	tempGCL = new TW3D::TW3DTempGCL(device);

	// -- Create the Command Allocators -- //

	//for (int i = 0; i < frameBufferCount; i++) {
		//device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, &commandAllocator);
	//}

	// -- Create a Command List -- //

	// create the command list with the first allocator
	commandList = TW3D::TW3DGraphicsCommandList::CreateDirect(device);
	//device->CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[frameIndex], &commandList);

	// -- Create a Fence & Fence Event -- //

	// create the fences
	for (int i = 0; i < frameBufferCount; i++) {
		fence[i] = new TW3D::TW3DFence(device);
	}

	// create root signature

	// create a root descriptor, which explains where to find the data for this root parameter
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	// create a descriptor range (descriptor table) and fill it out
	// this is a range of descriptors inside a descriptor heap
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1]; // only one range right now
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // this is a range of shader resource views (descriptors)
	descriptorTableRanges[0].NumDescriptors = 1; // we only have one texture right now, so the range is only 1
	descriptorTableRanges[0].BaseShaderRegister = 0; // start index of the shader registers in the range
	descriptorTableRanges[0].RegisterSpace = 0; // space 0. can usually be zero
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables

	// create a descriptor table
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges); // we only have one range
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0]; // the pointer to the beginning of our ranges array

	// create a root parameter for the root descriptor and fill it out
	D3D12_ROOT_PARAMETER  rootParameters[2]; // two root parameters
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // this is a constant buffer view root descriptor
	rootParameters[0].Descriptor = rootCBVDescriptor; // this is the root descriptor for this root parameter
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // our pixel shader will be the only shader accessing this parameter for now

	// fill out the parameter for our descriptor table. Remember it's a good idea to sort parameters by frequency of change. Our constant
	// buffer will be changed multiple times per frame, while our descriptor table will not be changed at all (in this tutorial)
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
	rootParameters[1].DescriptorTable = descriptorTable; // this is our descriptor table for this root parameter
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // our pixel shader will be the only shader accessing this parameter for now

	// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), // we have 2 root parameters
		rootParameters, // a pointer to the beginning of our root parameters array
		1, // we have one static sampler
		&sampler, // a pointer to our static sampler (array)
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* errorBuff; // a buffer holding the error data if any
	ID3DBlob* signature;
	TWU::SuccessAssert(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBuff));

	device->CreateRootSignature(signature, &rootSignature);


	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(2);
	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };


	pipelineState = new TW3D::TW3DPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		swapChain->GetDescription().SampleDesc,
		CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
		CD3DX12_BLEND_DESC(D3D12_DEFAULT),
		rootSignature,
		1);
	pipelineState->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	pipelineState->SetVertexShader("VertexShader.cso");
	pipelineState->SetPixelShader("PixelShader.cso");
	pipelineState->SetInputLayout(inputLayout);
	pipelineState->Create(device);
	
	

	// Create vertex buffer

	// a cube
	Vertex vList[] = {
		// front face
		{ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

		// right side face
		{  0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
		{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{  0.5f,  0.5f, -0.5f, 0.0f, 0.0f },

		// left side face
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

		// back face
		{  0.5f,  0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f },
		{  0.5f, -0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f },

		// top face
		{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f },
		{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f },

		// bottom face
		{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
		{  0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
		{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
	};

	int vBufferSize = sizeof(vList);

	vertexBuffer = new TW3D::TW3DResourceVB(device, vBufferSize, tempGCL);
	vertexBuffer->UpdateData(reinterpret_cast<BYTE*>(vList), vBufferSize);

	// Create index buffer

	// a quad (2 triangles)
	DWORD iList[] = {
		// ffront face
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle

		// left face
		4, 5, 6, // first triangle
		4, 7, 5, // second triangle

		// right face
		8, 9, 10, // first triangle
		8, 11, 9, // second triangle

		// back face
		12, 13, 14, // first triangle
		12, 15, 13, // second triangle

		// top face
		16, 17, 18, // first triangle
		16, 19, 17, // second triangle

		// bottom face
		20, 21, 22, // first triangle
		20, 23, 21, // second triangle
	};

	int iBufferSize = sizeof(iList);

	numCubeIndices = sizeof(iList) / sizeof(DWORD);

	// create default heap to hold index buffer

	indexBuffer = TW3D::TW3DResource::Create(device, iBufferSize, false);

	iBufferUploadHeap = TW3D::TW3DResource::Create(device, iBufferSize, true);

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(iList); // pointer to our index array
	indexData.RowPitch = iBufferSize; // size of all our index buffer
	indexData.SlicePitch = iBufferSize; // also the size of our index buffer

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	commandList->UpdateSubresources(indexBuffer, iBufferUploadHeap, &indexData);
	//UpdateSubresources(commandList, indexBuffer, iBufferUploadHeap, 0, 0, 1, &indexData);


	// transition the vertex buffer data from copy destination state to vertex buffer state
	commandList->ResourceBarrier(indexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	commandList->Close();
	commandQueue->ExecuteCommandList(commandList);

	// Create the depth/stencil buffer

	depthStencil = new TW3D::TW3DResourceDSV(device);
	depthStencil->Create(width, height);

	// create the constant buffer resource heap
	// We will update the constant buffer one or more times per frame, so we will use only an upload heap
	// unlike previously we used an upload heap to upload the vertex and index data, and then copied over
	// to a default heap. If you plan to use a resource for more than a couple frames, it is usually more
	// efficient to copy to a default heap where it stays on the gpu. In this case, our constant buffer
	// will be modified and uploaded at least once per frame, so we only use an upload heap

	// first we will create a resource heap (upload heap) for each frame for the cubes constant buffers
	// As you can see, we are allocating 64KB for each resource we create. Buffer resource heaps must be
	// an alignment of 64KB. We are creating 3 resources, one for each frame. Each constant buffer is 
	// only a 4x4 matrix of floats in this tutorial. So with a float being 4 bytes, we have 
	// 16 floats in one constant buffer, and we will store 2 constant buffers in each
	// heap, one for each cube, thats only 64x2 bits, or 128 bits we are using for each
	// resource, and each resource must be at least 64KB (65536 bits)
	for (int i = 0; i < frameBufferCount; ++i) {
		constantBufferUploadHeaps[i] = TW3D::TW3DResource::CreateCBStaging(device);

		//constantBufferUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

		ZeroMemory(&cbPerObject, sizeof(cbPerObject));

		CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (so end is less than or equal to begin)

		// map the resource heap to get a gpu virtual address to the beginning of the heap
		constantBufferUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&cbvGPUAddress[i]));

		// Because of the constant read alignment requirements, constant buffer views must be 256 bit aligned. Our buffers are smaller than 256 bits,
		// so we need to add spacing between the two buffers, so that the second buffer starts at 256 bits from the beginning of the resource heap.
		memcpy(cbvGPUAddress[i], &cbPerObject, sizeof(cbPerObject)); // cube1's constant buffer data
		memcpy(cbvGPUAddress[i] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject)); // cube2's constant buffer data
	}

	// load the image, create a texture resource and descriptor heap

	mainDescriptorHeap = TW3D::TW3DDescriptorHeap::CreateForSR(device, 1);

	texture = TW3D::TW3DResourceSV::Create2D(device, mainDescriptorHeap, L"D:\\тест.png", tempGCL);

	fence[frameIndex]->Flush(commandQueue);

	// we are done with image data now that we've uploaded it to the gpu, so free it up
	//delete imageData;

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = vBufferSize;

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 32-bit unsigned integer (this is what a dword is, double word, a word is 2 bytes)
	indexBufferView.SizeInBytes = iBufferSize;

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
	XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(45.0f*(3.14f / 180.0f), (float)width / (float)height, 0.1f, 1000.0f);
	XMStoreFloat4x4(&cameraProjMat, tmpMat);

	// set starting camera state
	cameraPosition = XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
	cameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	// build view matrix
	XMVECTOR cPos = XMLoadFloat4(&cameraPosition);
	XMVECTOR cTarg = XMLoadFloat4(&cameraTarget);
	XMVECTOR cUp = XMLoadFloat4(&cameraUp);
	tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
	XMStoreFloat4x4(&cameraViewMat, tmpMat);

	// set starting cubes position
	// first cube
	cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // set cube 1's position
	XMVECTOR posVec = XMLoadFloat4(&cube1Position); // create xmvector for cube1's position

	tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube1's position vector
	XMStoreFloat4x4(&cube1RotMat, XMMatrixIdentity()); // initialize cube1's rotation matrix to identity matrix
	XMStoreFloat4x4(&cube1WorldMat, tmpMat); // store cube1's world matrix

	// second cube
	cube2PositionOffset = XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
	posVec = XMLoadFloat4(&cube2PositionOffset) + XMLoadFloat4(&cube1Position); // create xmvector for cube2's position
																				// we are rotating around cube1 here, so add cube2's position to cube1

	tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube2's position offset vector
	XMStoreFloat4x4(&cube2RotMat, XMMatrixIdentity()); // initialize cube2's rotation matrix to identity matrix
	XMStoreFloat4x4(&cube2WorldMat, tmpMat); // store cube2's world matrix
}

void FlushGPU() {
	for (UINT n = 0; n < frameBufferCount; n++)
		fence[n]->Flush(commandQueue);
		
}

void UpdatePipeline() {
	commandList->Reset();

	commandList->SetPipelineState(pipelineState);

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

	// get a handle to the depth/stencil buffer
	//CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	commandList->SetRenderTarget(renderTargets[frameIndex], depthStencil);
	//commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	
	// Clear the render target by using the ClearRenderTargetView command
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRTV(renderTargets[frameIndex], clearColor);
	//commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// clear the depth/stencil buffer
	commandList->ClearDSVDepth(depthStencil, 1.0f);
	//commandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// set root signature
	commandList->SetGraphicsRootSignature(rootSignature); // set the root signature

	// set the descriptor heap
	commandList->SetDescriptorHeap(mainDescriptorHeap);

	// set the descriptor table to the descriptor heap (parameter 1, as constant buffer root descriptor is parameter index 0)
	commandList->SetGraphicsRootDescriptorTable(1, mainDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	commandList->SetViewport(&viewport); // set the viewports
	commandList->SetScissor(&scissorRect); // set the scissor rects
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	commandList->SetVertexBuffer(0, &vertexBufferView); // set the vertex buffer (using the vertex buffer view)
	commandList->SetIndexBuffer(&indexBufferView);

	// first cube

	// set cube1's constant buffer
	commandList->SetGraphicsRootCBV(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress());

	// draw first cube
	commandList->DrawIndexed(numCubeIndices);

	// second cube

	// set cube2's constant buffer. You can see we are adding the size of ConstantBufferPerObject to the constant buffer
	// resource heaps address. This is because cube1's constant buffer is stored at the beginning of the resource heap, while
	// cube2's constant buffer data is stored after (256 bits from the start of the heap).
	commandList->SetGraphicsRootCBV(0, constantBufferUploadHeaps[frameIndex]->GetGPUVirtualAddress() + ConstantBufferPerObjectAlignedSize);

	// draw second cube
	commandList->DrawIndexed(numCubeIndices);

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	commandList->Close();
}

void update() {
	// update app logic, such as moving the camera or figuring out what objects are in view

	// create rotation matrices
	XMMATRIX rotXMat = XMMatrixRotationX(0.01f);
	XMMATRIX rotYMat = XMMatrixRotationY(0.02f);
	XMMATRIX rotZMat = XMMatrixRotationZ(0.03f);

	// add rotation to cube1's rotation matrix and store it
	XMMATRIX rotMat = XMLoadFloat4x4(&cube1RotMat) * rotXMat * rotYMat * rotZMat;
	XMStoreFloat4x4(&cube1RotMat, rotMat);

	// create translation matrix for cube 1 from cube 1's position vector
	XMMATRIX translationMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube1Position));

	// create cube1's world matrix by first rotating the cube, then positioning the rotated cube
	XMMATRIX worldMat = rotMat * translationMat;

	// store cube1's world matrix
	XMStoreFloat4x4(&cube1WorldMat, worldMat);

	// update constant buffer for cube1
	// create the wvp matrix and store in constant buffer
	XMMATRIX viewMat = XMLoadFloat4x4(&cameraViewMat); // load view matrix
	XMMATRIX projMat = XMLoadFloat4x4(&cameraProjMat); // load projection matrix
	XMMATRIX wvpMat = XMLoadFloat4x4(&cube1WorldMat) * viewMat * projMat; // create wvp matrix
	XMMATRIX transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
	XMStoreFloat4x4(&cbPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

	// copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbvGPUAddress[frameIndex], &cbPerObject, sizeof(cbPerObject));

	// now do cube2's world matrix
	// create rotation matrices for cube2
	rotXMat = XMMatrixRotationX(0.03f);
	rotYMat = XMMatrixRotationY(0.02f);
	rotZMat = XMMatrixRotationZ(0.01f);

	// add rotation to cube2's rotation matrix and store it
	rotMat = rotZMat * (XMLoadFloat4x4(&cube2RotMat) * (rotXMat * rotYMat));
	XMStoreFloat4x4(&cube2RotMat, rotMat);

	// create translation matrix for cube 2 to offset it from cube 1 (its position relative to cube1
	XMMATRIX translationOffsetMat = XMMatrixTranslationFromVector(XMLoadFloat4(&cube2PositionOffset));

	// we want cube 2 to be half the size of cube 1, so we scale it by .5 in all dimensions
	XMMATRIX scaleMat = XMMatrixScaling(0.5f, 0.5f, 0.5f);

	// reuse worldMat. 
	// first we scale cube2. scaling happens relative to point 0,0,0, so you will almost always want to scale first
	// then we translate it. 
	// then we rotate it. rotation always rotates around point 0,0,0
	// finally we move it to cube 1's position, which will cause it to rotate around cube 1
	worldMat = scaleMat * translationOffsetMat * rotMat * translationMat;

	wvpMat = XMLoadFloat4x4(&cube2WorldMat) * viewMat * projMat; // create wvp matrix
	transposed = XMMatrixTranspose(wvpMat); // must transpose wvp matrix for the gpu
	XMStoreFloat4x4(&cbPerObject.wvpMat, transposed); // store transposed wvp matrix in constant buffer

	// copy our ConstantBuffer instance to the mapped constant buffer resource
	memcpy(cbvGPUAddress[frameIndex] + ConstantBufferPerObjectAlignedSize, &cbPerObject, sizeof(cbPerObject));

	// store cube2's world matrix
	XMStoreFloat4x4(&cube2WorldMat, worldMat);
}

void render() {
	fence[frameIndex]->Flush(commandQueue);
	frameIndex = swapChain->GetCurrentBufferIndex();

	UpdatePipeline(); // update the pipeline by sending commands to the commandqueue


	commandQueue->ExecuteCommandList(commandList);

	// present the current backbuffer
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

	for (UINT n = 0; n < frameBufferCount; n++)
		renderTargets[n]->Release();

	depthStencil->Release();

	swapChain->Resize(width, height);

	frameIndex = swapChain->GetCurrentBufferIndex();

	for (UINT n = 0; n < frameBufferCount; n++)
		renderTargets[n]->Create(swapChain->GetBuffer(n), n);

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

	// get swapchain out of full screen before exiting
	if (swapChain->GetFullscreen()) {
		swapChain->SetFullscreen(false);
	}

	delete factory;
	delete adapter;
	delete device;
	delete swapChain;

	delete tempGCL;

	delete commandQueue;
	delete rtvDescriptorHeap;
	delete commandList;

	//TWU::DXSafeRelease(commandAllocator);

	for (int i = 0; i < frameBufferCount; ++i) {
		delete renderTargets[i];
		delete constantBufferUploadHeaps[i];
		delete fence[i];
	};

	delete mainDescriptorHeap;
	
	//delete vBufferUploadHeap;
	delete iBufferUploadHeap;

	delete pipelineState;
	TWU::DXSafeRelease(rootSignature);
	delete vertexBuffer;
	delete indexBuffer;

	delete depthStencil;
	delete texture;

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug)))) {
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
#endif // _DEBUG
}

void TW3D::Start(const InitializeInfo& info) {
	width = info.width;
	height = info.height;
	title = info.title;
	fullscreen = info.fullscreen;
	vsync = info.vsync;

	init_window();
	init_dx12();

	main_loop();
	cleanup();
}