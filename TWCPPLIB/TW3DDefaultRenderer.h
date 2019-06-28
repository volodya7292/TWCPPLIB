#pragma once
#include "TW3DRenderer.h"
#include "TW3DRaytracer.h"

class TW3DDefaultRenderer : public TW3DRenderer {
public:
	TW3DDefaultRenderer() = default;
	~TW3DDefaultRenderer() final;
	void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height) final;
	void InitializeFrame(TW3DSCFrame* Frame) final;
	void Resize(TWT::uint Width, TWT::uint Height) final;
	void Update(float DeltaTime) final;
	void Execute(TW3DSCFrame* Frame) final;

	TW3DScene* LargeScaleScene = nullptr;

private:
	void CreateBlitResources();
	void CreateGBufferResources();
	void CreateRTResources();

	void record_g_buffer(TW3DCommandList* cl);

	void BlitOutput(TW3DCommandList* cl, TW3DRenderTarget* ColorOutput);

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
		GBUFFER_VERTEX_PREV_CAMERA_CB,
		GBUFFER_VERTEX_VMI_CB,
		GBUFFER_PIXEL_CAMERA_CB,
		GBUFFER_PIXEL_RENDERER_CB,
		GBUFFER_PIXEL_DIFFUSE_TEXTURE,
		GBUFFER_PIXEL_SPECULAR_TEXTURE,
		GBUFFER_PIXEL_EMISSION_TEXTURE,
		GBUFFER_PIXEL_NORMAL_TEXTURE,
	};
	TW3DGraphicsPipelineState *gbuffer_ps = nullptr;
	TW3DRenderTarget *g_position, *g_normal, *g_diffuse, *g_specular, *g_emission;
	TW3DTexture *g_depth;

	// Ray tracing compute
	// --------------------------------------------------------------------- 
	TW3DRaytracer* ray_tracer;
	float rt_scale = 0.25f;

	// Blit output
	// --------------------------------------------------------------------- 
	TW3DGraphicsPipelineState *blit_ps = nullptr;

	D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
	D3D12_RECT scissor = D3D12_RECT();
};