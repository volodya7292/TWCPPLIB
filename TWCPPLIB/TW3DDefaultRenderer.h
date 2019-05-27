#pragma once
#include "TW3DRenderer.h"

class TW3DDefaultRenderer : public TW3DRenderer {
public:
	TW3DDefaultRenderer() = default;
	~TW3DDefaultRenderer() final;
	void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height) final;
	void Resize(TWT::uint Width, TWT::uint Height) final;
	void Record(TWT::uint BackBufferIndex, TW3DRenderTarget* ColorOutput, TW3DTexture* DepthStencilOutput) final;
	void RecordBeforeExecution() final;
	void Update(float DeltaTime) final;
	void Execute(TWT::uint BackBufferIndex) final;

	TW3DScene* LargeScaleScene = nullptr;

private:
	void CreateBlitResources();
	void CreateGBufferResources();
	void CreateRTResources();

	void BlitOutput(TW3DGraphicsCommandList* cl, TW3DRenderTarget* ColorOutput, TW3DTexture* Depth);
	void RenderRecordGBuffer();

	TW3DGraphicsCommandList* g_cl = nullptr;    // GBuffer CL
	TW3DGraphicsCommandList* rt_cl = nullptr;   // Ray tracing CL

	// GBuffer render
	// --------------------------------------------------------------------- 
	enum GBufferRPIs {
		GBUFFER_VERTEX_CAMERA_CB,
		GBUFFER_VERTEX_VMI_CB,
		GBUFFER_PIXEL_CAMERA_CB,
		GBUFFER_PIXEL_VMISCALE_CONST,
		GBUFFER_PIXEL_ALBEDO_TEXTURE,
		GBUFFER_PIXEL_SPECULAR_TEXTURE
	};
	TW3DGraphicsPipelineState *gbuffer_ps = nullptr;
	TW3DRenderTarget *g_position, *g_normal, *g_diffuse, *g_specular;
	TW3DTexture* g_depth;

	// Ray tracing compute
	// --------------------------------------------------------------------- 
	enum RTRPIs {
		RT_GVB_BUFFER,
		RT_GNB_BUFFER,
		RT_SCENE_BUFFER,
		RT_L_GVB_BUFFER,
		RT_L_GNB_BUFFER,
		RT_L_SCENE_BUFFER,
		RT_OUTPUT_TEXTURE,
		RT_CAMERA_CB,
	};
	TW3DComputePipelineState *rt_ps = nullptr;
	TW3DTexture* rt_output = nullptr;

	// Blit output
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *blit_ps = nullptr;

	D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
	D3D12_RECT scissor = D3D12_RECT();
};