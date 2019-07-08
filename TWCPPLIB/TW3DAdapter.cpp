#include "pch.h"
#include "TW3DAdapter.h"

TW3DAdapter::TW3DAdapter(IDXGIAdapter4* Native, D3D_FEATURE_LEVEL FeatureLevel) :
	adapter(Native), feature_level(FeatureLevel) {

	DXGI_ADAPTER_DESC3 desc;
	TWU::SuccessAssert(adapter->GetDesc3(&desc), "TW3DAdapter::TW3DAdapter");

	description = TWT::WString(desc.Description);
	dedicated_video_memory = desc.DedicatedVideoMemory;
}

TW3DAdapter::~TW3DAdapter() {
	TWU::DXSafeRelease(adapter);
}

D3D_FEATURE_LEVEL TW3DAdapter::GetFeatureLevel() {
	return feature_level;
}

TWT::WString TW3DAdapter::GetDescription() {
	return description;
}

void TW3DAdapter::CreateDevice(ID3D12Device2** NativeDevice) {
	//UUID experimentalFeatures[] = { D3D12ExperimentalShaderModels };
	//TWU::SuccessAssert(D3D12EnableExperimentalFeatures(1, experimentalFeatures, nullptr, nullptr));
	TWU::SuccessAssert(D3D12CreateDevice(adapter, feature_level, IID_PPV_ARGS(NativeDevice)), "TW3DAdapter::CreateDevice");
}

std::vector<TW3DAdapter*> TW3DAdapter::ListAvailable(TW3DFactory* Factory, D3D_FEATURE_LEVEL FeatureLevel) {
	std::vector<ComPtr<IDXGIAdapter4>> native_adapters = Factory->ListAdapters(FeatureLevel);

	std::vector<TW3DAdapter*> adapters;
	for (auto native_adapter : native_adapters)
		adapters.emplace_back(new TW3DAdapter(native_adapter.Detach(), FeatureLevel));

	return adapters;
}
