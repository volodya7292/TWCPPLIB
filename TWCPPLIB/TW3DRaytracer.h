#pragma once
#include "TW3DScene.h"

class TW3DRaytracer {
public:
	TW3DRaytracer(TW3DResourceManager* ResourceManager);
	~TW3DRaytracer();

	void TraceRays(TW3DGraphicsCommandList* CommandList, TW3DScene* Scene, TW3DScene* LargeScaleScene);

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
		RT_RENDERERINFO_CB,
	};
	enum RootSignatureParamsSVGFTA {
		RT_CAMERA_CB1,
		
	};
	enum RootSignatureParamsSVGFEV {
		RT_CAMERA_CB2,
	};
	enum RootSignatureParamsSVGFWF {
		RT_CAMERA_CB3,
	};

	TW3DShader *rt_s, *svgf_ta_s, *svgf_ev_s, *svgf_wf_s;
	TW3DComputePipelineState *rt_ps, *svgf_ta_ps, *svgf_ev_ps, *svgf_wf_ps;
};