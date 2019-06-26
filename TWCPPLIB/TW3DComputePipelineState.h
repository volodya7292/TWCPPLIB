#pragma once
#include "TW3DRootSignature.h"
#include "TW3DShader.h"

class TW3DComputePipelineState {
public:
	TW3DComputePipelineState(TW3DRootSignature* RootSignature);
	~TW3DComputePipelineState();

	ID3D12PipelineState* Get();

	// BindingsChangePriorities: { binding_name, priority(default: 0) }     const std::vector<std::pair<TWT::String, TWT::uint>>& BindingsChangePriorities
	void SetShader(TW3DShader* Shader);
	void Create(TW3DDevice* Device);

	TW3DRootSignature* RootSignature;

private:
	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	ID3D12PipelineState* pipeline_state    = nullptr;
};