#include "pch.h"
#include "TW3DRootParameter.h"

TW3D::TW3DRootParameter::TW3DRootParameter(D3D12_ROOT_PARAMETER_TYPE Type, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register) {
	D3D12_ROOT_DESCRIPTOR descriptor = {};
	descriptor.RegisterSpace = 0;
	descriptor.ShaderRegister = Register;

	RootParameter.ParameterType = Type;
	RootParameter.Descriptor = descriptor;
	RootParameter.ShaderVisibility = ShaderVisibility;
}

TW3D::TW3DRootParameter::TW3DRootParameter(D3D12_SHADER_VISIBILITY ShaderVisibility, const D3D12_DESCRIPTOR_RANGE& DescriptorRange) {
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = 1;
	descriptorTable.pDescriptorRanges = &DescriptorRange;

	RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter.DescriptorTable = descriptorTable;
	RootParameter.ShaderVisibility = ShaderVisibility;
}

TW3D::TW3DRootParameter::TW3DRootParameter(D3D12_SHADER_VISIBILITY ShaderVisibility, const TWT::Vector<D3D12_DESCRIPTOR_RANGE>& DescriptorRanges) {
	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = DescriptorRanges.size();
	descriptorTable.pDescriptorRanges = DescriptorRanges.data();

	RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	RootParameter.DescriptorTable = descriptorTable;
	RootParameter.ShaderVisibility = ShaderVisibility;
}

D3D12_ROOT_PARAMETER TW3D::TW3DRootParameter::Get() {
	return RootParameter;
}

TW3D::TW3DRootParameter TW3D::TW3DRootParameter::CreateCBV(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility) {
	return TW3DRootParameter(D3D12_ROOT_PARAMETER_TYPE_CBV, ShaderVisibility, Register);
}