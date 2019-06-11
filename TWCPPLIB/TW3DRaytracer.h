#pragma once
#include "TW3DResourceManager.h"

class TW3DScene;

class TW3DRaytracer {
public:
	TW3DRaytracer(TW3DResourceManager* ResourceManager, TWT::uint2 RTSize);
	~TW3DRaytracer();

	void Resize(TWT::uint2 Size);

	void TraceRays(TW3DGraphicsCommandList* CL,
		TW3DRenderTarget* GPosition, TW3DRenderTarget* GDiffuse, TW3DRenderTarget* GSpecular, TW3DRenderTarget* GNormal, TW3DRenderTarget* GEmission, TW3DTexture* GDepth,
		TW3DTexture* DiffuseTexArr, TW3DTexture* EmissionTexArr, TW3DTexture* NormalTexArr, TW3DConstantBuffer* RendererInfoCB,
		TW3DScene* Scene, TW3DScene* LargeScaleScene = nullptr);

private:
	enum RootSignatureParamsRT {
		RT_GVB_BUFFER,
		RT_GNB_BUFFER,
		RT_SCENE_BUFFER,
		RT_LSB_BUFFER,
		RT_L_GVB_BUFFER,
		RT_L_GNB_BUFFER,
		RT_L_SCENE_BUFFER,
		RT_L_LSB_BUFFER,
		RT_DIFFUSE_TEXTURE,
		//RT_SPECULAR_TEXTURE,
		RT_EMISSION_TEXTURE,
		RT_NORMAL_TEXTURE,
		RT_G_POSITION_TEXTURE,
		RT_G_NORMAL_TEXTURE,
		RT_G_DIFFUSE_TEXTURE,
		RT_G_SPECULAR_TEXTURE,
		RT_G_EMISSION_TEXTURE,
		RT_DIRECT_TEXTURE,
		RT_DIRECT_ALBEDO_TEXTURE,
		RT_INDIRECT_TEXTURE,
		RT_INDIRECT_ALBEDO_TEXTURE,
		RT_INPUT_CONST,
		RT_CAMERA_CB,
		RT_RENDERERINFO_CB
	};
	enum RootSignatureParamsSVGFTA {
		SVGFTA_DIRECT,
		SVGFTA_INDIRECT,
		SVGFTA_PREV_DIRECT,
		SVGFTA_PREV_INDIRECT,
		SVGFTA_PREV_MOMENTS,
		SVGFTA_MOTION,
		SVGFTA_LINEAR_Z,
		SVGFTA_PREV_LINEAR_Z,
		SVGFTA_HISTORY_LENGTH
	};
	enum RootSignatureParamsSVGFEV {
		SVGFEV_DIRECT,
		SVGFEV_INDIRECT,
		SVGFEV_MOMENTS,
		SVGFEV_COMPACT_NORM_DEPTH,
		SVGFEV_HISTORY_LENGTH
	};
	enum RootSignatureParamsSVGFWF {
		SVGFWF_DIRECT,
		SVGFWF_INDIRECT,
		SVGFWF_COMPACT_NORM_DEPTH,
		SVGFWF_INPUT_DATA
	};

	TWT::uint2 size;

	TW3DShader *sq_s, *rt_s, *svgf_ta_s, *svgf_ev_s, *svgf_wf_s;
	TW3DComputePipelineState* rt_ps;
	TW3DGraphicsPipelineState *svgf_ta_ps, *svgf_ev_ps, *svgf_wf_ps;

	TW3DTexture *direct_tex, *indirect_tex, *direct_albedo_tex, *indirect_albedo_tex;
	TW3DFramebuffer *svgf_swap_fb[2], *svgf_filtered_fb;    // direct | indirect
	TW3DFramebuffer *svgf_ta_curr_fb, *svgf_ta_prev_fb;     // direct | indirect | moments | history length
};