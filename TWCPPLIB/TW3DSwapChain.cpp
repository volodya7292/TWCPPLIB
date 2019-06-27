#include "pch.h"
#include "TW3DSwapChain.h"

TW3DSwapChain::TW3DSwapChain(TW3DFactory* Factory, TW3DCommandQueue* CommandQueue, HWND HWND, TWT::uint Width, TWT::uint Height, bool VSync) :
	VSync(VSync), tearing(Factory->CheckTearingSupport())
{
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count

	desc = {};
	desc.Width = Width;
	desc.Height = Height;
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
	Factory->CreateSwapChainForHwnd(CommandQueue->Get(), HWND, &desc, &tempSwapChain);
	swap_chain = static_cast<IDXGISwapChain4*>(tempSwapChain);
}

TW3DSwapChain::~TW3DSwapChain() {
	TWU::DXSafeRelease(swap_chain);
}

DXGI_SWAP_CHAIN_DESC1 TW3DSwapChain::GetDescription() {
	return desc;
}

bool TW3DSwapChain::GetFullScreen() {
	BOOL fullscreen;
	TWU::SuccessAssert(swap_chain->GetFullscreenState(&fullscreen, nullptr), "TW3DSwapChain::GetFullscreen"s);
	return fullscreen;
}

void TW3DSwapChain::SetFullScreen(bool fullscreen) {
	TWU::SuccessAssert(swap_chain->SetFullscreenState(fullscreen, nullptr), "TW3DSwapChain::SetFullscreen "s + TWU::BoolStr(fullscreen));
}

TWT::uint TW3DSwapChain::GetCurrentBufferIndex() {
	return swap_chain->GetCurrentBackBufferIndex();
}

ID3D12Resource* TW3DSwapChain::GetBuffer(TWT::uint index) {
	ID3D12Resource* resource;
	swap_chain->GetBuffer(index, IID_PPV_ARGS(&resource));
	return resource;
}

void TW3DSwapChain::Resize(TWT::uint width, TWT::uint height) {
	swap_chain->ResizeBuffers(BufferCount, width, height, desc.Format, desc.Flags);
}

void TW3DSwapChain::Present() {
	swap_chain->Present(VSync ? 1 : 0, tearing && !VSync && !GetFullScreen() ? DXGI_PRESENT_ALLOW_TEARING : 0);
}
