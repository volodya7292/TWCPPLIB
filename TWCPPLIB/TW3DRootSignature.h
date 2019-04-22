#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DRootSignature {
	public:
		TW3DRootSignature(TWT::Bool AllowGeometryShader = false, TWT::Bool AllowVertexShader = true, TWT::Bool AllowPixelShader = true, TWT::Bool AllowInputAssembler = true);
		~TW3DRootSignature();

		ID3D12RootSignature* Get();

		void SetParameter(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, D3D12_ROOT_PARAMETER_TYPE DescriptorType);
		void SetParameterTable(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, D3D12_DESCRIPTOR_RANGE_TYPE DescriptorType, TWT::UInt DescriptorCount = 1);
		void SetParameterSRV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, TWT::UInt DescriptorCount = 1);
		void SetParameterUAV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, TWT::UInt DescriptorCount = 1);
		void SetParameterCBV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register);
		void SetParameterConstants(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, TWT::UInt Num32BitValues);
		

		void AddSampler(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility, D3D12_FILTER Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE AddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP, TWT::UInt MaxAnisotropy = 0);

		void Create(TW3D::TW3DDevice* device);

	private:
		ID3D12RootSignature* root_signature = nullptr;
		TWT::Vector<D3D12_ROOT_PARAMETER> root_parameters;
		TWT::Vector<D3D12_STATIC_SAMPLER_DESC> samplers;
		D3D12_ROOT_SIGNATURE_DESC desc;
		TWT::UInt parameter_count = 0;
	};
}