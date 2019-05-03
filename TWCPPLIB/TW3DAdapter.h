#pragma once
#include "TW3DUtils.h"
#include "TW3DFactory.h"

namespace TW3D {
	class TW3DAdapter {
	public:
		TW3DAdapter(IDXGIAdapter4* native_adapter, D3D_FEATURE_LEVEL featureLevel);
		TW3DAdapter(const TW3DAdapter &str) = default;
		~TW3DAdapter();

		D3D_FEATURE_LEVEL GetFeatureLevel();
		TWT::WString GetDescription();

		void CreateDevice(ID3D12Device2** device);

		static std::vector<TW3D::TW3DAdapter*> ListAvailable(TW3DFactory* factory, D3D_FEATURE_LEVEL featureLevel);

	private:
		IDXGIAdapter4* native_adapter;
		D3D_FEATURE_LEVEL featureLevel;

		TWT::WString description;
		TWT::UInt64 dedicatedVideoMemory;
	};
}

