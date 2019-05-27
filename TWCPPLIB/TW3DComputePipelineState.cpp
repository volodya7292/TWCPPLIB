#include "pch.h"
#include "TW3DComputePipelineState.h"

TW3DComputePipelineState::TW3DComputePipelineState(TW3DRootSignature* RootSignature) :
	RootSignature(RootSignature)
{
	desc.pRootSignature = RootSignature->Get();
}

TW3DComputePipelineState::~TW3DComputePipelineState() {
	TWU::DXSafeRelease(pipeline_state);
	if (RootSignature->DestroyOnPipelineDestroy)
		delete RootSignature;
}

ID3D12PipelineState* TW3DComputePipelineState::Get() {
	return pipeline_state;
}

void TW3DComputePipelineState::SetShader(TW3DShader* Shader) {
	desc.CS = Shader->GetByteCode();
}

void TW3DComputePipelineState::Create(TW3DDevice* Device) {
	Device->CreateComputePipelineState(&desc, &pipeline_state);
	pipeline_state->SetName(L"TW3DComputePipelineState");
}
