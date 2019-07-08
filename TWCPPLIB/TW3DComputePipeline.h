#pragma once
#include "TW3DRootSignature.h"
#include "TW3DShader.h"

class TW3DComputePipeline {
public:
	TW3DComputePipeline(TW3DRootSignature* RootSignature);
	~TW3DComputePipeline();

	void SetShader(TW3DShader* Shader);
	void Create(TW3DDevice* Device);

	ID3D12PipelineState* Native = nullptr;
	TW3DRootSignature* RootSignature;

private:
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
};