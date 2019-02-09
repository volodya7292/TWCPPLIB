#include "pch.h"
#include "TW3DRootSignature.h"
#include "TW3DRootParameter.h"

TW3D::TW3DRootSignature::TW3DRootSignature(D3D12_ROOT_SIGNATURE_FLAGS SignatureFlags) {
	Desc.Flags = SignatureFlags;
	//Samplers.resize(SamplerCount);

	/*D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS*/
}

TW3D::TW3DRootSignature::~TW3DRootSignature() {
	TWU::DXSafeRelease(RootSignature);
}

ID3D12RootSignature* TW3D::TW3DRootSignature::Get() {
	return RootSignature;
}

void TW3D::TW3DRootSignature::AddParameter(TW3DRootParameter Parameter) {
	Parameter.SetIndex(RootParameters.size());
	RootParameters.emplace_back(Parameter);
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
	TWT::Vector<D3D12_ROOT_PARAMETER> params;
	for (TW3DRootParameter& param : RootParameters)
		params.push_back(param.Get());

	Desc.NumParameters = params.size();
	Desc.pParameters = params.data();
	Desc.NumStaticSamplers = Samplers.size();
	Desc.pStaticSamplers = Samplers.data();

	ID3DBlob* errorBuff;
	ID3DBlob* signature;
	TWU::SuccessAssert(D3D12SerializeRootSignature(&Desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errorBuff));
	
	if (errorBuff) {
		TWT::Char* errors = static_cast<TWT::Char*>(errorBuff->GetBufferPointer());
		TWU::CPrintln(errors);
	}

	device->CreateRootSignature(signature, &RootSignature);
}
