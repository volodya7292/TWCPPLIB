#pragma once
#include "TW3DUtils.h"
#include "TW3DFactory.h"

namespace TW3D {
	class TW3DAdapter {
	public:
		TW3DAdapter(IDXGIAdapter4* native_adapter, D3D_FEATURE_LEVEL featureLevel);
		TW3DAdapter(const TW3DAdapter &str) = default;
		~TW3DAdapter();

		void CreateDevice(ID3D12Device5** device);

		TW3DAdapter& operator = (TW3DAdapter t);

		static std::vector<TW3D::TW3DAdapter*> ListAvailable(TW3DFactory* factory, D3D_FEATURE_LEVEL featureLevel);

		const D3D_FEATURE_LEVEL featureLevel;

	private:
		IDXGIAdapter4* native_adapter;
	};
}

