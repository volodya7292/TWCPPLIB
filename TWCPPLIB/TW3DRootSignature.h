#pragma once
#include "TW3DDevice.h"

struct TW3DRSRootParameter {
	TWT::uint Index;
	D3D12_ROOT_PARAMETER Parameter;
};

class TW3DRootSignature {
public:
	TW3DRootSignature(TW3DDevice* Device, std::vector<TW3DRSRootParameter> const& RootParameters,
		std::vector<D3D12_STATIC_SAMPLER_DESC> const& StaticSamplers, bool VS = true, bool PS = true, bool GS = false, bool IA = true);
	TW3DRootSignature(TW3DDevice* Device, std::vector<TW3DRSRootParameter> const& RootParameters,
		bool VS = true, bool PS = true, bool GS = false, bool IA = true);
	~TW3DRootSignature();

	bool DestroyOnPipelineDestroy = true;

	ID3D12RootSignature* Native;
};

TW3DRSRootParameter TW3DRPTexture(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess = false);
TW3DRSRootParameter TW3DRPBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess = false);
TW3DRSRootParameter TW3DRPConstantBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register);
TW3DRSRootParameter TW3DRPConstants(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, TWT::uint Num32BitValues);
D3D12_STATIC_SAMPLER_DESC TW3DStaticSampler(D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_FILTER Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
	D3D12_TEXTURE_ADDRESS_MODE AddressMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP, TWT::uint MaxAnisotropy = 0);