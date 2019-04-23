#include "pch.h"
#include "TW3DRootSignature.h"

TW3D::TW3DRootSignature::TW3DRootSignature(TWT::Bool AllowGeometryShader, TWT::Bool AllowVertexShader, TWT::Bool AllowPixelShader, TWT::Bool AllowInputAssembler) {
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

	if (!AllowGeometryShader)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	if (!AllowVertexShader)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
	if (!AllowPixelShader)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	if (AllowInputAssembler)
		desc.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	root_parameters.resize(64);
}

TW3D::TW3DRootSignature::~TW3DRootSignature() {
	TWU::DXSafeRelease(root_signature);
}

ID3D12RootSignature* TW3D::TW3DRootSignature::Get() {
	return root_signature;
}

void TW3D::TW3DRootSignature::SetParameter(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, D3D12_ROOT_PARAMETER_TYPE Type) {
	D3D12_ROOT_DESCRIPTOR descriptor = {};
	descriptor.RegisterSpace = 0;
	descriptor.ShaderRegister = Register;

	root_parameters[Index] = {};
	root_parameters[Index].ParameterType = Type;
	root_parameters[Index].Descriptor = descriptor;
	root_parameters[Index].ShaderVisibility = ShaderVisibility;

	parameter_count++;
}

void TW3D::TW3DRootSignature::SetParameterTable(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, D3D12_DESCRIPTOR_RANGE_TYPE DescriptorType, TWT::UInt DescriptorCount) {
	D3D12_DESCRIPTOR_RANGE* range = new CD3DX12_DESCRIPTOR_RANGE(DescriptorType, DescriptorCount, Register);

	root_parameters[Index] = {};
	root_parameters[Index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	root_parameters[Index].DescriptorTable = CD3DX12_ROOT_DESCRIPTOR_TABLE(1, range);
	root_parameters[Index].ShaderVisibility = ShaderVisibility;

	parameter_count++;
}

void TW3D::TW3DRootSignature::SetParameterSRV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, TWT::UInt DescriptorCount) {
	SetParameterTable(Index, ShaderVisibility, Register, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, DescriptorCount);
}

void TW3D::TW3DRootSignature::SetParameterUAV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register) {
	SetParameter(Index, ShaderVisibility, Register, D3D12_ROOT_PARAMETER_TYPE_UAV);
}

void TW3D::TW3DRootSignature::SetParameterCBV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register) {
	SetParameter(Index, ShaderVisibility, Register, D3D12_ROOT_PARAMETER_TYPE_CBV);
}

void TW3D::TW3DRootSignature::SetParameterConstants(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, TWT::UInt Num32BitValues) {
	root_parameters[Index] = {};
	root_parameters[Index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	root_parameters[Index].Constants = CD3DX12_ROOT_CONSTANTS(Num32BitValues, Register);
	root_parameters[Index].ShaderVisibility = ShaderVisibility;

	parameter_count++;
}

void TW3D::TW3DRootSignature::AddSampler(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility, D3D12_FILTER Filter, D3D12_TEXTURE_ADDRESS_MODE AddressMode, TWT::UInt MaxAnisotropy) {
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

	samplers.push_back(sampler);
}

void TW3D::TW3DRootSignature::Create(TW3D::TW3DDevice* device) {
	desc.NumParameters = parameter_count;
	desc.pParameters = root_parameters.data();
	desc.NumStaticSamplers = static_cast<UINT>(samplers.size());
	desc.pStaticSamplers = samplers.data();

	ID3DBlob* errorBuff;
	ID3DBlob* signature;
	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBuff);

	for (size_t i = 0; i < parameter_count; i++)
		if (root_parameters[i].ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[] root_parameters[i].DescriptorTable.pDescriptorRanges;

	if (errorBuff) {
		TWT::Char* errors = static_cast<TWT::Char*>(errorBuff->GetBufferPointer());
		TWU::CPrintln(errors);
		throw std::runtime_error("Root signature serialization failure");
	}

	device->CreateRootSignature(signature, &root_signature);
}
