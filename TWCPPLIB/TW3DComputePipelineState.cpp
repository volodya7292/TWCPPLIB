#include "pch.h"
#include "TW3DComputePipelineState.h"

TW3D::TW3DComputePipelineState::TW3DComputePipelineState(TW3DRootSignature* RootSignature) :
	RootSignature(RootSignature)
{
	desc.pRootSignature = RootSignature->Get();
}

TW3D::TW3DComputePipelineState::~TW3DComputePipelineState() {
	TWU::DXSafeRelease(pipeline_state);
	delete RootSignature;
}

ID3D12PipelineState* TW3D::TW3DComputePipelineState::Get() {
	return pipeline_state;
}

void TW3D::TW3DComputePipelineState::SetShader(const std::string& Filename) {
	TWT::Int size;
	TWT::Byte* data = TWU::ReadFileBytes(Filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	desc.CS = bytecode;
}

void TW3D::TW3DComputePipelineState::Create(TW3DDevice* Device) {
	Device->CreateComputePipelineState(&desc, &pipeline_state);
}
