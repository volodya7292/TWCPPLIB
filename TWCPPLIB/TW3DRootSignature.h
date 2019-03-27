#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DRootSignature {
	public:
		TW3DRootSignature(TWT::UInt RootParamCount, D3D12_ROOT_SIGNATURE_FLAGS SignatureFlags);
		~TW3DRootSignature();

		ID3D12RootSignature* Get();

		void SetParameter(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, D3D12_ROOT_PARAMETER_TYPE Type);
		void SetParameterSV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register);
		void SetParameterCBV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register);
		

		void AddSampler(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility, D3D12_FILTER Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE AddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP, TWT::UInt MaxAnisotropy = 0);

		void Create(TW3D::TW3DDevice* device);

	private:
		ID3D12RootSignature* RootSignature;
		TWT::Vector<D3D12_ROOT_PARAMETER> RootParameters;
		TWT::Vector<D3D12_STATIC_SAMPLER_DESC> Samplers;
		D3D12_ROOT_SIGNATURE_DESC Desc;
	};
}