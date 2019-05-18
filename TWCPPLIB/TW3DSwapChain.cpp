#include "pch.h"
#include "TW3DSwapChain.h"

TW3DSwapChain::TW3DSwapChain(TW3DFactory* factory, TW3DCommandQueue* commandQueue, HWND hwnd, TWT::uint width, TWT::uint height, bool vsync) :
	VSync(vsync), tearing(factory->CheckTearingSupport())
{
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count

	desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = false;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.BufferCount = BufferCount;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SampleDesc = sampleDesc;
	desc.Flags = tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	IDXGISwapChain1* tempSwapChain;
	factory->CreateSwapChainForHwnd(commandQueue->Get(), hwnd, &desc, &tempSwapChain);
	swapChain = static_cast<IDXGISwapChain4*>(tempSwapChain);
}

TW3DSwapChain::~TW3DSwapChain() {
	TWU::DXSafeRelease(swapChain);
}

DXGI_SWAP_CHAIN_DESC1 TW3DSwapChain::GetDescription() {
	return desc;
}

bool TW3DSwapChain::GetFullscreen() {
	BOOL fullscreen;
	TWU::SuccessAssert(swapChain->GetFullscreenState(&fullscreen, nullptr), "TW3DSwapChain::GetFullscreen"s);
	return fullscreen;
}

void TW3DSwapChain::SetFullscreen(bool fullscreen) {
	TWU::SuccessAssert(swapChain->SetFullscreenState(fullscreen, nullptr), "TW3DSwapChain::SetFullscreen "s + TWU::BoolStr(fullscreen));
}

TWT::uint TW3DSwapChain::GetCurrentBufferIndex() {
	return swapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource* TW3DSwapChain::GetBuffer(TWT::uint index) {
	ID3D12Resource* resource;
	swapChain->GetBuffer(index, IID_PPV_ARGS(&resource));
	return resource;
}

void TW3DSwapChain::Resize(TWT::uint width, TWT::uint height) {
	swapChain->ResizeBuffers(BufferCount, width, height, desc.Format, desc.Flags);
}

void TW3DSwapChain::Present() {
	swapChain->Present(VSync ? 1 : 0, tearing && !VSync && !GetFullscreen() ? DXGI_PRESENT_ALLOW_TEARING : 0);
}
