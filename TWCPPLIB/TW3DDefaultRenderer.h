#pragma once
#include "TW3DRenderer.h"

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
	void CreateGBufferResources();
	void CreateRTResources();

	void record_g_buffer_objects(TW3DCommandList* cl);
	void record_g_buffer(TW3DSCFrame* frame, TW3DCommandList* cl);

	void BlitOutput(TW3DCommandList* cl, TW3DRenderTarget* ColorOutput);

	TW3DShader *g_pixel_s, *g_vertex_s, *g_geom_s;

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
		//GBUFFER_VERTEX_PREV_CAMERA_CB,
		GBUFFER_VERTEX_RENDERER_CB,
		GBUFFER_VERTEX_VMI_CB,
		GBUFFER_PIXEL_CAMERA_CB,
		GBUFFER_PIXEL_RENDERER_CB,
		GBUFFER_PIXEL_DIFFUSE_TEXTURE,
		GBUFFER_PIXEL_SPECULAR_TEXTURE,
		GBUFFER_PIXEL_EMISSION_TEXTURE,
		GBUFFER_PIXEL_NORMAL_TEXTURE,
		GBUFFER_PIXEL_MATERIAL_BUFFER,
		GBUFFER_GEOMETRY_MATRICES,
	};
	struct CameraCubeMatrices {
		TWT::float4x4 proj_view[6];
	};

	TW3DGraphicsPipeline *gbuffer_ps = nullptr;
	TW3DRenderTarget *g_position, *g_normal, *g_diffuse, *g_specular, *g_emission;
	TW3DTexture* g_depth;

	TW3DCommandBuffer* objs_cmd_buffer;

	TW3DBuffer* material_buffer;
	TW3DConstantBuffer* cb_camera_matrices;


	D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
	D3D12_RECT scissor = D3D12_RECT();
};