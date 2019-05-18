#pragma once
#include "TW3DDevice.h"

class TW3DRootSignature {
public:
	TW3DRootSignature(bool AllowGeometryShader = false, bool AllowVertexShader = true, bool AllowPixelShader = true, bool AllowInputAssembler = true);
	~TW3DRootSignature();

	ID3D12RootSignature* Get();

	void SetParameter(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_ROOT_PARAMETER_TYPE DescriptorType);
	void SetParameterTable(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_DESCRIPTOR_RANGE_TYPE DescriptorType, TWT::uint DescriptorCount = 1);
	void SetParameterSRV(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, TWT::uint DescriptorCount = 1);
	void SetParameterUAVBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register);
	void SetParameterUAVTexture(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register);
	void SetParameterCBV(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register);
	void SetParameterConstants(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, TWT::uint Num32BitValues);


	void AddSampler(TWT::uint Register, D3D12_SHADER_VISIBILITY ShaderVisibility, D3D12_FILTER Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE AddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP, TWT::uint MaxAnisotropy = 0);

	void Create(TW3DDevice* device);

	bool DestroyOnPipelineDestroy = true;

private:
	ID3D12RootSignature* root_signature = nullptr;
	TWT::Vector<D3D12_ROOT_PARAMETER> root_parameters;
	TWT::Vector<D3D12_STATIC_SAMPLER_DESC> samplers;
	D3D12_ROOT_SIGNATURE_DESC desc;
	TWT::uint parameter_count = 0;
};