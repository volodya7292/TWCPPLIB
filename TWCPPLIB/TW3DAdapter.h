#pragma once
#include "TW3DUtils.h"
#include "TW3DFactory.h"

class TW3DAdapter {
public:
	TW3DAdapter(IDXGIAdapter4* Native, D3D_FEATURE_LEVEL FeatureLevel);
	TW3DAdapter(TW3DAdapter const& Adapter) = default;
	~TW3DAdapter();

	D3D_FEATURE_LEVEL GetFeatureLevel();
	TWT::WString GetDescription();

	void CreateDevice(ID3D12Device2** NativeDevice);

	static std::vector<TW3DAdapter*> ListAvailable(TW3DFactory* Factory, D3D_FEATURE_LEVEL FeatureLevel);

private:
	IDXGIAdapter4* adapter;
	D3D_FEATURE_LEVEL feature_level;

	TWT::WString description;
	TWT::uint64 dedicated_video_memory;
};