#pragma once
#include "TW3DResourceManager.h"

class TW3DScene;

class TW3DRaytracer {
public:
	TW3DRaytracer(TW3DResourceManager* ResourceManager, TWT::uint2 GSize, TWT::uint2 RTSize);
	~TW3DRaytracer();

	void Resize(TWT::uint2 GSize, TWT::uint2 RTSize);

	// CL : Compute command list
	void Render(TW3DCommandList* CL,
		TW3DRenderTarget* GPosition, TW3DRenderTarget* GDiffuse, TW3DRenderTarget* GSpecular, TW3DRenderTarget* GNormal, TW3DRenderTarget* GEmission, TW3DTexture* GDepth,
		TW3DTexture* DiffuseTexArr, TW3DTexture* EmissionTexArr, TW3DTexture* NormalTexArr, TW3DConstantBuffer* RendererInfoCB,
		TW3DScene* Scene, TW3DScene* LargeScaleScene = nullptr);
	void Tonemap(TW3DCommandList* CL);

	TW3DRenderTarget *svgf_mo_vec_rt, *svgf_compact_rt, *svgf_prev_compact_rt;
	TW3DTexture *direct_in, *indirect_in;
	TW3DTexture* Output;

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
		//RT_G_EMISSION_TEXTURE,
		RT_DIRECT_TEXTURE,
		RT_INDIRECT_TEXTURE,
		//RT_G_DATA_TEXTURE,
		RT_INPUT_CONST,
		RT_CAMERA_CB,
		RT_RENDERERINFO_CB
	};
	enum RootSignatureParamsSVGFTA {
		SVGFTA_DIRECT,
		SVGFTA_INDIRECT,
		SVGFTA_DIRECT_OUT,
		SVGFTA_INDIRECT_OUT,
		SVGFTA_FILTERED_DIRECT,
		SVGFTA_FILTERED_INDIRECT,
		SVGFTA_MOTION,
		SVGFTA_COMPACT_DATA,
		SVGFTA_PREV_COMPACT_DATA,
	};
	enum RootSignatureParamsSVGFWF {
		SVGFWF_DIRECT,
		SVGFWF_INDIRECT,
		SVGFWF_DIRECT_OUT,
		SVGFWF_INDIRECT_OUT,
		SVGFWF_INPUT_DATA,
		SVGFWF_FINAL_OUT,
		SVGFWF_PREV_DIRECT,
		SVGFWF_PREV_INDIRECT,
		SVGFWF_DETAIL_SUM_DIRECT,
		SVGFWF_DETAIL_SUM_INDIRECT,
		SVGFWF_COMPACT_DATA,
		SVGFWF_G_DIFFUSE_TEXTURE,
		SVGFWF_G_EMISSION_TEXTURE,
	};
	enum RootSignatureParamsTMO {
		TMO_TEXTURE,
	};

	void denoise(TW3DCommandList* CL);

	TWT::uint2 size;

	TW3DShader *sq_s, *rt_s, *svgf_ta_s, *svgf_wf_s, *tmo_s;
	TW3DComputePipelineState *rt_ps, *svgf_ta_ps, *svgf_wf_ps, *tmo_ps;

	TW3DTexture *temporal_history;
	TW3DTexture *detail_sum_direct, *detail_sum_indirect;
	TW3DTexture *svgf_swap_direct[2], *svgf_swap_indirect[2], *svgf_filtered_direct, *svgf_filtered_indirect;

	TW3DRenderTarget *g_diffuse, *g_emission;

	const TWT::uint SPATIAL_FILTER_MAX_ITERATIONS = 5;
};