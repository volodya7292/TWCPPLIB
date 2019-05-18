#include "pch.h"
#include "TW3DFactory.h"

TW3DFactory::TW3DFactory(TWT::UInt flags) {
	TWU::SuccessAssert(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory)), "TW3DFactory::TW3DFactory"s);
}

TW3DFactory::~TW3DFactory() {
	TWU::DXSafeRelease(factory);
}

std::vector<ComPtr<IDXGIAdapter4>> TW3DFactory::ListAdapters(D3D_FEATURE_LEVEL featureLevel) {
	ComPtr<IDXGIAdapter1> adapter1;
	ComPtr<IDXGIAdapter4> adapter4;
	std::vector<ComPtr<IDXGIAdapter4>> adapters;

	for (TWT::UInt adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &adapter1) != DXGI_ERROR_NOT_FOUND; adapterIndex++) {
		DXGI_ADAPTER_DESC1 desc;
		TWU::SuccessAssert(adapter1->GetDesc1(&desc), "TW3DFactory::ListAdapters, adapter1->GetDesc1 "s + adapterIndex);

		//if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), featureLevel, _uuidof(ID3D12Device), nullptr))) {
		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) && SUCCEEDED(D3D12CreateDevice(adapter1.Get(), featureLevel, _uuidof(ID3D12Device), nullptr))) {
			TWU::SuccessAssert(adapter1.As(&adapter4), "TW3DFactory::ListAdapters, adapter1.As "s + adapterIndex);
			adapters.push_back(adapter4);
			break;
		}
	}
	
	return adapters;
}

void TW3DFactory::CreateSwapChainForHwnd(ID3D12CommandQueue* commandQueue, HWND hwnd, const DXGI_SWAP_CHAIN_DESC1* desc, IDXGISwapChain1** swapChain) {
	TWU::SuccessAssert(factory->CreateSwapChainForHwnd(commandQueue, hwnd, desc, nullptr, nullptr, swapChain), "TW3DFactory::CreateSwapChainForHwnd, factory->CreateSwapChainForHwnd"s);
	TWU::SuccessAssert(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER), "TW3DFactory::CreateSwapChainForHwnd, factory->MakeWindowAssociation"s);
}

void TW3DFactory::CheckFeatureSupport(DXGI_FEATURE feature, void *featureSupportData, TWT::UInt featureSupportDataSize) {
	//TWU::SuccessAssert(factory->CheckFeatureSupport(feature, featureSupportData, featureSupportDataSize));
}

TWT::Bool TW3DFactory::CheckTearingSupport() {
	BOOL allowTearing = false;
	//TWU::SuccessAssert(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing)));
	return allowTearing;
}
