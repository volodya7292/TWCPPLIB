#include "pch.h"
#include "TW3DComputePipeline.h"

TW3DComputePipeline::TW3DComputePipeline(TW3DRootSignature* RootSignature) :
	RootSignature(RootSignature)
{
	desc.pRootSignature = RootSignature->Native;
}

TW3DComputePipeline::~TW3DComputePipeline() {
	TWU::DXSafeRelease(Native);
}

void TW3DComputePipeline::SetShader(TW3DShader* Shader) {
	desc.CS = Shader->GetByteCode();
}

void TW3DComputePipeline::Create(TW3DDevice* Device) {
	Device->CreateComputePipelineState(&desc, &Native);
}