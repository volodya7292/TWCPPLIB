#include "pch.h"
#include "TW3DFactory.h"

TW3D::TW3DFactory::TW3DFactory(TWT::UInt flags) {
	TWU::ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory)));
}

TW3D::TW3DFactory::~TW3DFactory() {
	TWU::DXSafeRelease(factory);
}

std::vector<ComPtr<IDXGIAdapter4>> TW3D::TW3DFactory::ListAdapters(D3D_FEATURE_LEVEL featureLevel) {
	ComPtr<IDXGIAdapter1> adapter1;
	ComPtr<IDXGIAdapter4> adapter4;
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	for (TWT::UInt adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &adapter1) != DXGI_ERROR_NOT_FOUND; adapterIndex++) {
		DXGI_ADAPTER_DESC1 desc;
		TWU::ThrowIfFailed(adapter1->GetDesc1(&desc));

		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) && SUCCEEDED(D3D12CreateDevice(adapter1.Get(), featureLevel, _uuidof(ID3D12Device), nullptr))) {
			TWU::ThrowIfFailed(adapter1.As(&adapter4));
			adapters.push_back(adapter4);
			break;
		}
	}

	return adapters;
}

void TW3D::TW3DFactory::CreateSwapChainForHwnd(ID3D12CommandQueue* commandQueue, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapChain) {
	TWU::ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue, hwnd, desc, nullptr, nullptr, swapChain));
	TWU::ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
}

