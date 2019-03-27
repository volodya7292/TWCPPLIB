#include "pch.h"
#include "TW3DRootSignature.h"

TW3D::TW3DRootSignature::TW3DRootSignature(TWT::UInt RootParamCount, D3D12_ROOT_SIGNATURE_FLAGS SignatureFlags) {
	Desc.Flags = SignatureFlags;
	RootParameters.resize(RootParamCount);
}

TW3D::TW3DRootSignature::~TW3DRootSignature() {
	TWU::DXSafeRelease(RootSignature);
}

ID3D12RootSignature* TW3D::TW3DRootSignature::Get() {
	return RootSignature;
}

void TW3D::TW3DRootSignature::SetParameter(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register, D3D12_ROOT_PARAMETER_TYPE Type) {
	D3D12_ROOT_DESCRIPTOR descriptor = {};
	descriptor.RegisterSpace = 0;
	descriptor.ShaderRegister = Register;

	RootParameters[Index] = {};
	RootParameters[Index].ParameterType = Type;
	RootParameters[Index].Descriptor = descriptor;
	RootParameters[Index].ShaderVisibility = ShaderVisibility;
}

void TW3D::TW3DRootSignature::SetParameterSV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register) {
	D3D12_DESCRIPTOR_RANGE* range = new CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, Register);

	RootParameters[Index] = {};
	RootParameters[Index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameters[Index].DescriptorTable = CD3DX12_ROOT_DESCRIPTOR_TABLE(1, range);
	RootParameters[Index].ShaderVisibility = ShaderVisibility;
}

void TW3D::TW3DRootSignature::SetParameterCBV(TWT::UInt Index, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register) {
	SetParameter(Index, ShaderVisibility, Register, D3D12_ROOT_PARAMETER_TYPE_CBV);
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

	Samplers.push_back(sampler);
}

void TW3D::TW3DRootSignature::Create(TW3D::TW3DDevice* device) {
	Desc.NumParameters = RootParameters.size();
	Desc.pParameters = RootParameters.data();
	Desc.NumStaticSamplers = Samplers.size();
	Desc.pStaticSamplers = Samplers.data();

	ID3DBlob* errorBuff;
	ID3DBlob* signature;
	D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBuff);
	
	for (const D3D12_ROOT_PARAMETER& param : RootParameters) {
		delete[] param.DescriptorTable.pDescriptorRanges;
	}

	if (errorBuff) {
		TWT::Char* errors = static_cast<TWT::Char*>(errorBuff->GetBufferPointer());
		TWU::CPrintln(errors);
		throw std::runtime_error("Root signature serialization failure");
	}

	device->CreateRootSignature(signature, &RootSignature);
}
