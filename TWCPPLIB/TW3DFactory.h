#pragma once
#include "TW3DUtils.h"

namespace TW3D {
	class TW3DFactory {
	public:
		TW3DFactory(TWT::UInt flags);
		~TW3DFactory();

		std::vector<ComPtr<IDXGIAdapter4>> ListAdapters(D3D_FEATURE_LEVEL featureLevel);
		void CreateSwapChainForHwnd(ID3D12CommandQueue* commandQueue, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapChain);

	private:
		IDXGIFactory7* factory;
	};
}