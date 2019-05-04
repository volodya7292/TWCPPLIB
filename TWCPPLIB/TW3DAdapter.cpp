#include "pch.h"
#include "TW3DAdapter.h"

TW3D::TW3DAdapter::TW3DAdapter(IDXGIAdapter4* native_adapter, D3D_FEATURE_LEVEL featureLevel) :
	native_adapter(native_adapter), featureLevel(featureLevel)
{
	DXGI_ADAPTER_DESC3 desc;
	TWU::SuccessAssert(native_adapter->GetDesc3(&desc), "TW3DAdapter::TW3DAdapter");
	
	description = TWT::WString(desc.Description);
	dedicatedVideoMemory = desc.DedicatedVideoMemory;
}

TW3D::TW3DAdapter::~TW3DAdapter() {
	TWU::DXSafeRelease(native_adapter);
}

D3D_FEATURE_LEVEL TW3D::TW3DAdapter::GetFeatureLevel() {
	return featureLevel;
}

TWT::WString TW3D::TW3DAdapter::GetDescription() {
	return description;
}

void TW3D::TW3DAdapter::CreateDevice(ID3D12Device2** device) {
	//UUID experimentalFeatures[] = { D3D12ExperimentalShaderModels };
	//TWU::SuccessAssert(D3D12EnableExperimentalFeatures(1, experimentalFeatures, nullptr, nullptr));
	TWU::SuccessAssert(D3D12CreateDevice(native_adapter, featureLevel, IID_PPV_ARGS(device)), "TW3DAdapter::CreateDevice");
}

std::vector<TW3D::TW3DAdapter*> TW3D::TW3DAdapter::ListAvailable(TW3DFactory* factory, D3D_FEATURE_LEVEL featureLevel) {
	std::vector<ComPtr<IDXGIAdapter4>> native_adapters = factory->ListAdapters(featureLevel);

	std::vector<TW3DAdapter*> adapters;
	for (auto native_adapter : native_adapters)
		adapters.emplace_back(new TW3DAdapter(native_adapter.Detach(), featureLevel));

	return adapters;
}
