#pragma once
#include "TW3DDevice.h"
#include "TW3DRootParameter.h"

namespace TW3D {
	class TW3DRootSignature {
	public:
		TW3DRootSignature(D3D12_ROOT_SIGNATURE_FLAGS SignatureFlags);
		~TW3DRootSignature();

		ID3D12RootSignature* Get();

		void AddParameter(TW3DRootParameter Parameter);

		void AddSampler(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility, D3D12_FILTER Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE AddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP, TWT::UInt MaxAnisotropy = 0);

		void Create(TW3D::TW3DDevice* device);

	private:
		ID3D12RootSignature* RootSignature;
		TWT::Vector<TW3DRootParameter> RootParameters;
		TWT::Vector<D3D12_STATIC_SAMPLER_DESC> Samplers;
		D3D12_ROOT_SIGNATURE_DESC Desc;
	};
}