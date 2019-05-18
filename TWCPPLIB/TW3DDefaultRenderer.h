#pragma once
#include "TW3DRenderer.h"

class TW3DDefaultRenderer : public TW3DRenderer {
public:
	TW3DDefaultRenderer() = default;
	virtual ~TW3DDefaultRenderer();
	void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height);
	void Resize(TWT::uint Width, TWT::uint Height);
	void Record(TWT::uint BackBufferIndex, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStesncilOutput);
	void RecordBeforeExecution();
	void Update(float DeltaTime);
	void Execute(TWT::uint BackBufferIndex);

private:
	void CreateBlitResources();
	void CreateGBufferResources();
	void CreateRTResources();

	void BlitOutput(TW3DGraphicsCommandList* cl, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* Depth);

	TW3DResourceUAV* rt_output = nullptr;

	TW3DGraphicsCommandList* rt_cl = nullptr;   // Ray tracing CL

	// GBuffer render
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *gbuffer_ps = nullptr;

	// Ray tracing compute
	// --------------------------------------------------------------------- 
	TW3DComputePipelineState *rt_ps = nullptr;

	// Blit output
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *blit_ps = nullptr;

	D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
	D3D12_RECT scissor = D3D12_RECT();
};