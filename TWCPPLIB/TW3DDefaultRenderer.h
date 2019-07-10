#pragma once
#include "TW3DRenderer.h"

struct TW3DDefaultRendererSettigns {
	enum GraphicsType {
		GRAPHICS_DEFAULT,   // front render target
		GRAPHICS_IMPROVED,  // Front + Cube render target
		GRAPHICS_REALISTIC  // Front render target, ray tracing
	} GraphicsType = GRAPHICS_DEFAULT;

	struct GraphicsDefault {
		enum ShadowsQuality {
			SHADOWS_QUALITY_LOW,
			SHADOWS_QUALITY_MEDIUM,
			SHADOWS_QUALITY_HIGH,
		} ShadowsQuality;
	} DefaultGraphics;

	struct GraphicsImproved {
		float BackScale = 0.25f; // Cube render target scale

		enum ShadowsQuality {
			SHADOWS_QUALITY_LOW,
			SHADOWS_QUALITY_MEDIUM,
			SHADOWS_QUALITY_HIGH,
		} ShadowsQuality;
	} ImprovedGraphics;

	struct GraphicsRealistic {
		// Non-implementable
	};

	float Scale = 1.0f;

};

class TW3DDefaultRenderer : public TW3DRenderer {
public:
	TW3DDefaultRenderer() = default;
	~TW3DDefaultRenderer() final;
	void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint2 Size) final;
	void InitializeFrame(TW3DSCFrame* Frame) final;
	void Resize(TWT::uint2 Size) final;
	void Update(float DeltaTime) final;
	void Execute(TW3DSCFrame* Frame) final;

	TW3DScene* LargeScaleScene = nullptr;

private:
	void CreateGBufferResources();

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
	struct ObjectCmd {
		D3D12_GPU_VIRTUAL_ADDRESS vb;
		D3D12_DRAW_ARGUMENTS draw;
	};

	TW3DGraphicsPipeline *cube_gbuffer_ps, *front_gbuffer_ps;

	TW3DFramebuffer *cube_gbuffer, *front_gbuffer;
	float cube_gbuffer_scale = 1.0f;

	TW3DCommandSignature* objs_cmd_sign;
	TW3DCommandBuffer* objs_cmd_buffer;

	TW3DBuffer* material_buffer;
	TW3DConstantBuffer* cb_camera_matrices;
};