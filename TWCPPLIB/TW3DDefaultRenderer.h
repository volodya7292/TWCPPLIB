#pragma once
#include "TW3DRenderer.h"

class TW3DDefaultRenderer : public TW3DRenderer {
public:
	TW3DDefaultRenderer() = default;
	virtual ~TW3DDefaultRenderer();
	void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height);
	void Resize(TWT::uint Width, TWT::uint Height);
	void Record(TWT::uint BackBufferIndex, TW3DRenderTarget* ColorOutput, TW3DTexture* DepthStesncilOutput);
	void RecordBeforeExecution();
	void Update(float DeltaTime);
	void Execute(TWT::uint BackBufferIndex);

private:
	void CreateBlitResources();
	void CreateGBufferResources();
	void CreateRTResources();

	void BlitOutput(TW3DGraphicsCommandList* cl, TW3DRenderTarget* ColorOutput, TW3DTexture* Depth);
	void RenderRecordGBuffer();

	TW3DGraphicsCommandList* g_cl = nullptr;   // Ray tracing CL
	TW3DGraphicsCommandList* rt_cl = nullptr;   // Ray tracing CL

	// GBuffer render
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *gbuffer_ps = nullptr;
	TW3DRenderTarget* g_albedo = nullptr;
	TW3DTexture* g_depth = nullptr;

	// Ray tracing compute
	// --------------------------------------------------------------------- 
	TW3DComputePipelineState *rt_ps = nullptr;
	TW3DTexture* rt_output = nullptr;

	// Blit output
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *blit_ps = nullptr;

	D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
	D3D12_RECT scissor = D3D12_RECT();
};