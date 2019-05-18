#pragma once
#include "TW3DUtils.h"

class TW3DFactory {
public:
	TW3DFactory(TWT::UInt flags);
	~TW3DFactory();

	std::vector<ComPtr<IDXGIAdapter4>> ListAdapters(D3D_FEATURE_LEVEL featureLevel);
	void CreateSwapChainForHwnd(ID3D12CommandQueue* commandQueue, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapChain);

	void CheckFeatureSupport(DXGI_FEATURE feature, void *featureSupportData, TWT::UInt featureSupportDataSize);
	TWT::Bool CheckTearingSupport();

private:
	IDXGIFactory2* factory;
};