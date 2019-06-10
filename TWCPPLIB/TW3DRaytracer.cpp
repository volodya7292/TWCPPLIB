#include "pch.h"
#include "TW3DRaytracer.h"
#include "TW3DScene.h"
#include "CompiledShaders/RayTrace.c.h"
#include "CompiledShaders/ScreenQuad.v.h"
#include "CompiledShaders/SVGFTemporalAccumulation.p.h"
#include "CompiledShaders/SVGFEstimateVariance.p.h"
#include "CompiledShaders/SVGFWaveletFilter.p.h"

TW3DRaytracer::TW3DRaytracer(TW3DResourceManager* ResourceManager) {
	auto device = ResourceManager->GetDevice();

	sq_s = new TW3DShader(TW3DCompiledShader(ScreenQuad_VertexByteCode), "ScreenQuad");
	rt_s = new TW3DShader(TW3DCompiledShader(RayTrace_ByteCode), "RayTrace");
	svgf_ta_s = new TW3DShader(TW3DCompiledShader(SVGFTemporalAccumulation_PixelByteCode), "SVGFTemporalAccumulation");
	svgf_ev_s = new TW3DShader(TW3DCompiledShader(SVGFEstimateVariance_PixelByteCode), "SVGFEstimateVariance");
	svgf_wf_s = new TW3DShader(TW3DCompiledShader(SVGFWaveletFilter_PixelByteCode), "SVGFWaveletFilter");


	auto rs = new TW3DRootSignature(device,
		{
			// Ray tracing resources
			TW3DRPBuffer(RT_GVB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("gvb"s)),
			TW3DRPBuffer(RT_SCENE_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("scene"s)),
			TW3DRPBuffer(RT_GNB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("gnb"s)),
			TW3DRPBuffer(RT_LSB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("lsb"s)),
			TW3DRPBuffer(RT_L_GVB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("l_gvb"s)),
			TW3DRPBuffer(RT_L_SCENE_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("l_scene"s)),
			TW3DRPBuffer(RT_L_GNB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("l_gnb"s)),
			TW3DRPBuffer(RT_L_LSB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("l_lsb"s)),

			// Texture resources
			TW3DRPTexture(RT_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("diffuse_tex"s)),
			//TW3DRPTexture(RT_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("specular_tex"s)),
			TW3DRPTexture(RT_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("emission_tex"s)),
			TW3DRPTexture(RT_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("normal_tex"s)),

			// GBuffer
			TW3DRPTexture(RT_G_POSITION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("g_position"s)),
			TW3DRPTexture(RT_G_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("g_normal"s)),
			TW3DRPTexture(RT_G_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("g_diffuse"s)),
			TW3DRPTexture(RT_G_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("g_specular"s)),
			TW3DRPTexture(RT_G_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("g_emission"s)),

			// Ray tracing output resources
			TW3DRPTexture(RT_DIRECT_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_direct"s), true),
			TW3DRPTexture(RT_DIRECT_ALBEDO_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_direct_albedo"s), true),
			TW3DRPTexture(RT_INDIRECT_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_indirect"s), true),
			TW3DRPTexture(RT_INDIRECT_ALBEDO_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_indirect_albedo"s), true),

			// Scene data
			TW3DRPConstantBuffer(RT_CAMERA_CB, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("camera"s)),
			TW3DRPConstants(RT_INPUT_CONST, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("input"s), 3),
			TW3DRPConstantBuffer(RT_RENDERERINFO_CB, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("renderer"s)),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		false, false, false, false
		);

	rt_ps = new TW3DComputePipelineState(rs);
	rt_ps->SetShader(rt_s);
	rt_ps->Create(device);


	rs = new TW3DRootSignature(device,
		{
			TW3DRPTexture(SVGFTA_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFTA_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFTA_PREV_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_prev_direct"s)),
			TW3DRPTexture(SVGFTA_PREV_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_prev_indirect"s)),
			TW3DRPTexture(SVGFTA_PREV_MOMENTS, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_prev_moments"s)),
			TW3DRPTexture(SVGFTA_MOTION, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_motion"s)),
			TW3DRPTexture(SVGFTA_LINEAR_Z, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_linear_z"s)),
			TW3DRPTexture(SVGFTA_PREV_LINEAR_Z, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_prev_linear_z"s)),
			TW3DRPTexture(SVGFTA_HISTORY_LENGTH, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_history_length"s))
		},
		true, true, false, false
		);

	svgf_ta_ps = new TW3DGraphicsPipelineState(rs);
	svgf_ta_ps->SetRTVFormat(0, TWT::RGBA32Float);
	svgf_ta_ps->SetRTVFormat(1, TWT::RGBA32Float);
	svgf_ta_ps->SetRTVFormat(2, TWT::RGBA32Float);
	svgf_ta_ps->SetRTVFormat(3, TWT::R16Float);
	svgf_ta_ps->SetVertexShader(sq_s);
	svgf_ta_ps->SetPixelShader(svgf_ta_s);
	svgf_ta_ps->Create(device);


	rs = new TW3DRootSignature(device,
		{
			TW3DRPTexture(SVGFEV_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ev_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFEV_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ev_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFEV_MOMENTS, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ev_s->GetRegister("g_moments"s)),
			TW3DRPTexture(SVGFEV_COMPACT_NORM_DEPTH, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ev_s->GetRegister("g_compact_norm_depth"s)),
			TW3DRPTexture(SVGFEV_HISTORY_LENGTH, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ev_s->GetRegister("g_history_length"s)),
		},
		true, true, false, false
	);

	svgf_ev_ps = new TW3DGraphicsPipelineState(rs);
	svgf_ev_ps->SetRTVFormat(0, TWT::RGBA32Float);
	svgf_ev_ps->SetRTVFormat(1, TWT::RGBA32Float);
	svgf_ev_ps->SetVertexShader(sq_s);
	svgf_ev_ps->SetPixelShader(svgf_ev_s);
	svgf_ev_ps->Create(device);


	rs = new TW3DRootSignature(device,
		{
			TW3DRPTexture(SVGFWF_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFWF_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFWF_COMPACT_NORM_DEPTH, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_compact_norm_depth"s)),
			TW3DRPConstants(SVGFWF_INPUT_DATA, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("input_data"s), 1)
		},
		true, true, false, false
		);

	svgf_wf_ps = new TW3DGraphicsPipelineState(rs);
	svgf_wf_ps->SetRTVFormat(0, TWT::RGBA32Float);
	svgf_wf_ps->SetRTVFormat(1, TWT::RGBA32Float);
	svgf_wf_ps->SetVertexShader(sq_s);
	svgf_wf_ps->SetPixelShader(svgf_wf_s);
	svgf_wf_ps->Create(device);
}

TW3DRaytracer::~TW3DRaytracer() {
	delete sq_s;
	delete rt_s;
	delete svgf_ta_s;
	delete svgf_ev_s;
	delete svgf_wf_s;
	delete rt_ps;
	delete svgf_ta_ps;
	delete svgf_ev_ps;
	delete svgf_wf_ps;
}

void TW3DRaytracer::TraceRays(TW3DGraphicsCommandList* CommandList, TW3DScene* Scene, TW3DScene* LargeScaleScene) {
}
