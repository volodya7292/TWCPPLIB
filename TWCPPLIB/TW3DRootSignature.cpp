#include "pch.h"
#include "TW3DRootSignature.h"

//TW3DRootSignature::TW3DRootSignature(bool AllowGeometryShader, bool AllowVertexShader, bool AllowPixelShader, bool AllowInputAssembler) {
//	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
//
//	if (!AllowGeometryShader)
//		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
//	if (!AllowVertexShader)
//		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
//	if (!AllowPixelShader)
//		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
//	if (AllowInputAssembler)
//		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	root_parameters.resize(64);
//}
//
//TW3DRootSignature::~TW3DRootSignature() {
//	TWU::DXSafeRelease(root_signature);
//}
//
//ID3D12RootSignature* TW3DRootSignature::Get() {
//	return root_signature;
//}
//
//void TW3DRootSignature::SetParameter(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_ROOT_PARAMETER_TYPE Type) {
//	D3D12_ROOT_DESCRIPTOR descriptor = {};
//	descriptor.RegisterSpace = 0;
//	descriptor.ShaderRegister = Register;
//
//	root_parameters[Index] = {};
//	root_parameters[Index].ParameterType = Type;
//	root_parameters[Index].Descriptor = descriptor;
//	root_parameters[Index].ShaderVisibility = ShaderVisibility;
//
//	parameter_count++;
//}
//
//void TW3DRootSignature::SetParameterTable(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, D3D12_DESCRIPTOR_RANGE_TYPE DescriptorType, TWT::uint DescriptorCount) {
//	D3D12_DESCRIPTOR_RANGE* range = new CD3DX12_DESCRIPTOR_RANGE(DescriptorType, DescriptorCount, Register);
//
//	root_parameters[Index] = {};
//	root_parameters[Index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	root_parameters[Index].DescriptorTable = CD3DX12_ROOT_DESCRIPTOR_TABLE(1, range);
//	root_parameters[Index].ShaderVisibility = ShaderVisibility;
//
//	parameter_count++;
//}
//
//void TW3DRootSignature::SetParameterSRV(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, TWT::uint DescriptorCount) {
//	SetParameterTable(Index, ShaderVisibility, Register, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DescriptorCount);
//}
//
//void TW3DRootSignature::SetParameterUAVBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register) {
//	SetParameter(Index, ShaderVisibility, Register, D3D12_ROOT_PARAMETER_TYPE_UAV);
//}
//
//void TW3DRootSignature::SetParameterUAVTexture(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register) {
//	SetParameterTable(Index, ShaderVisibility, Register, D3D12_DESCRIPTOR_RANGE_TYPE_UAV);
//}
//
//void TW3DRootSignature::SetParameterCBV(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register) {
//	SetParameter(Index, ShaderVisibility, Register, D3D12_ROOT_PARAMETER_TYPE_CBV);
//}
//
//void TW3DRootSignature::SetParameterConstants(TWT::uint Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::uint Register, TWT::uint Num32BitValues) {
//	root_parameters[Index] = {};
//	root_parameters[Index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
//	root_parameters[Index].Constants = CD3DX12_ROOT_CONSTANTS(Num32BitValues, Register);
//	root_parameters[Index].ShaderVisibility = ShaderVisibility;
//
//	parameter_count++;
//}
//
//void TW3DRootSignature::AddSampler(TWT::uint Register, D3D12_SHADER_VISIBILITY ShaderVisibility, D3D12_FILTER Filter, D3D12_TEXTURE_ADDRESS_MODE AddressMode, TWT::uint MaxAnisotropy) {
//	D3D12_STATIC_SAMPLER_DESC sampler = {};
//	sampler.Filter = Filter;
//	sampler.AddressU = AddressMode;
//	sampler.AddressV = AddressMode;
//	sampler.AddressW = AddressMode;
//	sampler.MipLODBias = 0;
//	sampler.MaxAnisotropy = MaxAnisotropy;
//	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
//	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
//	sampler.MinLOD = 0.0f;
//	sampler.MaxLOD = D3D12_FLOAT32_MAX;
//	sampler.ShaderRegister = Register;
//	sampler.RegisterSpace = 0;
//	sampler.ShaderVisibility = ShaderVisibility;
//
//	samplers.push_back(sampler);
//}
//
//void TW3DRootSignature::Create(TW3DDevice* device) {
//	desc.NumParameters = parameter_count;
//	desc.pParameters = root_parameters.data();
//	desc.NumStaticSamplers = static_cast<UINT>(samplers.size());
//	desc.pStaticSamplers = samplers.data();
//
//	ID3DBlob* errorBuff;
//	ID3DBlob* signature;
//	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBuff);
//
//	for (size_t i = 0; i < parameter_count; i++)
//		if (root_parameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
//			delete[] root_parameters[i].DescriptorTable.pDescriptorRanges;
//
//	if (errorBuff) {
//		char* errors = static_cast<char*>(errorBuff->GetBufferPointer());
//		TWU::CPrintln(errors);
//		throw std::runtime_error("Root signature serialization failure");
//	}
//
//	device->CreateRootSignature(signature, &root_signature);
//}

TW3DRootSignature::TW3DRootSignature(TW3DDevice* Device, std::vector<D3D12_ROOT_PARAMETER> RootParameters,
	std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers, bool VS, bool PS, bool GS, bool IA)
{
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

	desc.NumParameters = static_cast<TWT::uint>(RootParameters.size());
	desc.pParameters = RootParameters.data();
	desc.NumStaticSamplers = static_cast<TWT::uint>(StaticSamplers.size());
	desc.pStaticSamplers = StaticSamplers.data();

	ID3DBlob* errorBuff;
	ID3DBlob* signature;
	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBuff);

	for (TWT::uint i = 0; i < desc.NumParameters; i++)
		if (RootParameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[] RootParameters[i].DescriptorTable.pDescriptorRanges;

	if (errorBuff) {
		char* errors = static_cast<char*>(errorBuff->GetBufferPointer());
		TWU::TW3DLogError(errors);
	}

	Device->CreateRootSignature(signature, &root_signature);
}

TW3DRootSignature::TW3DRootSignature(TW3DDevice* Device, std::vector<D3D12_ROOT_PARAMETER> RootParameters, bool VS, bool PS, bool GS, bool IA) :
	TW3DRootSignature(Device, RootParameters, {}, VS, PS, GS, IA)
{
}

TW3DRootSignature::~TW3DRootSignature() {
	TWU::DXSafeRelease(root_signature);
}

ID3D12RootSignature* TW3DRootSignature::Get() {
	return root_signature;
}

D3D12_ROOT_PARAMETER TW3DRPTexture(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess) {
	CD3DX12_ROOT_PARAMETER param = {};
	param.InitAsDescriptorTable(1, new CD3DX12_DESCRIPTOR_RANGE(UAVAccess ? D3D12_DESCRIPTOR_RANGE_TYPE_UAV : D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, Register), Visibility);
	return param;
}

D3D12_ROOT_PARAMETER TW3DRPBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, bool UAVAccess) {
	CD3DX12_ROOT_PARAMETER param = {};
	if (UAVAccess)
		param.InitAsUnorderedAccessView(Register, 0, Visibility);
	else
		param.InitAsShaderResourceView(Register, 0, Visibility);
	return param;
}

D3D12_ROOT_PARAMETER TW3DRPConstantBuffer(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register) {
	CD3DX12_ROOT_PARAMETER param = {};
	param.InitAsConstantBufferView(Register, 0, Visibility);
	return param;
}

D3D12_ROOT_PARAMETER TW3DRPConstants(TWT::uint Index, D3D12_SHADER_VISIBILITY Visibility, TWT::uint Register, TWT::uint Num32BitValues) {
	CD3DX12_ROOT_PARAMETER param = {};
	param.InitAsConstants(Num32BitValues, Register, 0, Visibility);
	return param;
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
