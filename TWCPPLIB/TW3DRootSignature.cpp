#include "pch.h"
#include "TW3DRootSignature.h"

TW3DRootSignature::TW3DRootSignature(TW3DDevice* Device, std::vector<TW3DRSRootParameter> const& RootParameters,
	std::vector<D3D12_STATIC_SAMPLER_DESC> const& StaticSamplers, bool VS, bool PS, bool GS, bool IA) {
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

	if (!GS)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	if (!VS)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	if (!PS)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	if (IA)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	std::vector<D3D12_ROOT_PARAMETER> ordered_params(RootParameters.size());
	for (auto const& v : RootParameters)
		ordered_params[v.Index] = v.Parameter;

	desc.NumParameters = static_cast<TWT::uint>(ordered_params.size());
	desc.pParameters = ordered_params.data();
	desc.NumStaticSamplers = static_cast<TWT::uint>(StaticSamplers.size());
	desc.pStaticSamplers = StaticSamplers.data();

	ID3DBlob* errorBuff;
	ID3DBlob* signature;
	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBuff);

	for (TWT::uint i = 0; i < desc.NumParameters; i++)
		if (ordered_params[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[] ordered_params[i].DescriptorTable.pDescriptorRanges;

	if (errorBuff) {
		char* errors = static_cast<char*>(errorBuff->GetBufferPointer());
		TWU::TW3DLogError(errors);
	}

	Device->CreateRootSignature(signature, &Native);
}

TW3DRootSignature::TW3DRootSignature(TW3DDevice* Device, std::vector<TW3DRSRootParameter> const& RootParameters, bool VS, bool PS, bool GS, bool IA) :
	TW3DRootSignature(Device, RootParameters, {}, VS, PS, GS, IA)
{
}

TW3DRootSignature::~TW3DRootSignature() {
	TWU::DXSafeRelease(Native);
}


TW3DRSRootParameter TW3DRPTexture(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess) {
	CD3DX12_ROOT_PARAMETER param = {};
	param.InitAsDescriptorTable(1, new CD3DX12_DESCRIPTOR_RANGE(UAVAccess ? D3D12_DESCRIPTOR_RANGE_TYPE_UAV : D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, Register), Visibility);
	return { Index, param };
}

TW3DRSRootParameter TW3DRPBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess) {
	CD3DX12_ROOT_PARAMETER param = {};
	if (UAVAccess)
		param.InitAsUnorderedAccessView(Register, 0, Visibility);
	else
		param.InitAsShaderResourceView(Register, 0, Visibility);
	return { Index, param };
}

TW3DRSRootParameter TW3DRPConstantBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register) {
	CD3DX12_ROOT_PARAMETER param = {};
	param.InitAsConstantBufferView(Register, 0, Visibility);
	return { Index, param };
}

TW3DRSRootParameter TW3DRPConstants(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, TWT::uint Num32BitValues) {
	CD3DX12_ROOT_PARAMETER param = {};
	param.InitAsConstants(Num32BitValues, Register, 0, Visibility);
	return { Index, param };
}

D3D12_STATIC_SAMPLER_DESC TW3DStaticSampler(D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_FILTER Filter, D3D12_TEXTURE_ADDRESS_MODE AddressMode, TWT::uint MaxAnisotropy) {
	D3D12_STATIC_SAMPLER_DESC sampler = {};
	sampler.Filter = Filter;
	sampler.AddressU = AddressMode;
	sampler.AddressV = AddressMode;
	sampler.AddressW = AddressMode;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = MaxAnisotropy;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = Register;
	sampler.RegisterSpace = 0;
	sampler.ShaderVisibility = ShaderVisibility;
	return sampler;
}
