#pragma once
#include "TW3DDevice.h"

class TW3DRootSignature {
public:
	TW3DRootSignature(TW3DDevice* Device, std::vector<D3D12_ROOT_PARAMETER> RootParameters,
		std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers, bool VS = true, bool PS = true, bool GS = false, bool IA = true);
	TW3DRootSignature(TW3DDevice* Device, std::vector<D3D12_ROOT_PARAMETER> RootParameters,
		bool VS = true, bool PS = true, bool GS = false, bool IA = true);
	~TW3DRootSignature();

	ID3D12RootSignature* Get();

	bool DestroyOnPipelineDestroy = true;

private:
	ID3D12RootSignature* root_signature = nullptr;
};

D3D12_ROOT_PARAMETER TW3DRPTexture(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess = false);
D3D12_ROOT_PARAMETER TW3DRPBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess = false);
D3D12_ROOT_PARAMETER TW3DRPConstantBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register);
D3D12_ROOT_PARAMETER TW3DRPConstants(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, TWT::uint Num32BitValues);
D3D12_STATIC_SAMPLER_DESC TW3DStaticSampler(D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_FILTER Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
	D3D12_TEXTURE_ADDRESS_MODE AddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP, TWT::uint MaxAnisotropy = 0);