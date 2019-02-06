#include "pch.h"
#include "TW3DSwapChain.h"

TW3D::TW3DSwapChain::TW3DSwapChain(TW3D::TW3DFactory* factory, TW3DCommandQueue* commandQueue, HWND hwnd, TWT::UInt width, TWT::UInt height, TWT::Bool vsync) :
	vsync(vsync), tearing(factory->CheckTearingSupport())
{
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = false;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.BufferCount = bufferCount; // number of buffers we have
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	desc.SampleDesc = sampleDesc; // our multi-sampling description
	desc.Flags = tearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	IDXGISwapChain1* tempSwapChain;
	factory->CreateSwapChainForHwnd(commandQueue->Get(), hwnd, &desc, &tempSwapChain);
	swapChain = static_cast<IDXGISwapChain4*>(tempSwapChain);
}

TW3D::TW3DSwapChain::~TW3DSwapChain() {
	TWU::DXSafeRelease(swapChain);
}

TWT::Bool TW3D::TW3DSwapChain::GetFullscreen() {
	BOOL fullscreen;
	TWU::ThrowIfFailed(swapChain->GetFullscreenState(&fullscreen, nullptr));
	return fullscreen;
}

void TW3D::TW3DSwapChain::SetFullscreen(TWT::Bool fullscreen) {
	TWU::ThrowIfFailed(swapChain->SetFullscreenState(fullscreen, nullptr));
}

TWT::UInt TW3D::TW3DSwapChain::GetCurrentBufferIndex() {
	return swapChain->GetCurrentBackBufferIndex();
}

ID3D12Resource* TW3D::TW3DSwapChain::GetBuffer(TWT::UInt index) {
	ID3D12Resource* resource;
	swapChain->GetBuffer(index, IID_PPV_ARGS(&resource));
	return resource;
}

void TW3D::TW3DSwapChain::Resize(TWT::UInt width, TWT::UInt height) {
	swapChain->ResizeBuffers(bufferCount, width, height, desc.Format, desc.Flags);
}

void TW3D::TW3DSwapChain::Present() {
	swapChain->Present(vsync ? 1 : 0, tearing && !vsync ? DXGI_PRESENT_ALLOW_TEARING : 0);
}
