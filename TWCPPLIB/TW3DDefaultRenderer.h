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

	// Renderer resources
	// --------------------------------------------------------------------- 
	const TWT::uint material_count = 64;
	TWT::DefaultRendererInfoCB info;
	TW3DTexture *diffuse_texarr, *specular_texarr, *emission_texarr, *normal_texarr;
	TW3DConstantBuffer* info_cb;

	// GBuffer render
	// --------------------------------------------------------------------- 
	enum GBufferRPIs {
		GBUFFER_VERTEX_CAMERA_CB,
		GBUFFER_VERTEX_VMI_CB,
		GBUFFER_PIXEL_CAMERA_CB,
		GBUFFER_PIXEL_VMISCALE_CONST,
		GBUFFER_PIXEL_DIFFUSE_TEXTURE,
		GBUFFER_PIXEL_SPECULAR_TEXTURE,
		GBUFFER_PIXEL_EMISSION_TEXTURE,
		GBUFFER_PIXEL_NORMAL_TEXTURE,
	};
	TW3DGraphicsPipelineState *gbuffer_ps = nullptr;
	TW3DRenderTarget *g_position, *g_normal, *g_diffuse, *g_specular, *g_emission;
	TW3DTexture* g_depth;

	// Ray tracing compute
	// --------------------------------------------------------------------- 
	enum RTRPIs {
		RT_GVB_BUFFER,
		RT_GNB_BUFFER,
		RT_SCENE_BUFFER,
		RT_LSB_BUFFER,
		RT_L_GVB_BUFFER,
		RT_L_GNB_BUFFER,
		RT_L_SCENE_BUFFER,
		RT_L_LSB_BUFFER,
		RT_DIFFUSE_TEXTURE,
		RT_SPECULAR_TEXTURE,
		RT_EMISSION_TEXTURE,
		RT_NORMAL_TEXTURE,
		RT_G_POSITION_TEXTURE,
		RT_G_NORMAL_TEXTURE,
		RT_G_DIFFUSE_TEXTURE,
		RT_G_SPECULAR_TEXTURE,
		RT_G_EMISSION_TEXTURE,
		RT_OUTPUT_TEXTURE,
		RT_INPUT_CONST,
		RT_CAMERA_CB,
		RT_RENDERERINFO_CB,
	};
	TW3DComputePipelineState *rt_ps = nullptr;
	TW3DTexture* rt_output = nullptr;

	// Blit output
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *blit_ps = nullptr;

	D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
	D3D12_RECT scissor = D3D12_RECT();
};