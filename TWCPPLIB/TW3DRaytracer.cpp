#include "pch.h"
#include "TW3DRaytracer.h"
#include "TW3DScene.h"
#include "CompiledShaders/RayTrace.c.h"
#include "CompiledShaders/ScreenQuad.v.h"
#include "CompiledShaders/RTDTemporalAccumulation.p.h"
#include "CompiledShaders/RTDWaveletFilter.p.h"

TW3DRaytracer::TW3DRaytracer(TW3DResourceManager* ResourceManager, TWT::uint2 GSize, TWT::uint2 RTSize) :
	size(RTSize) {
	auto device = ResourceManager->GetDevice();

	sq_s = new TW3DShader(TW3DCompiledShader(ScreenQuad_VertexByteCode), "ScreenQuad");
	rt_s = new TW3DShader(TW3DCompiledShader(RayTrace_ByteCode), "RayTrace");
	svgf_ta_s = new TW3DShader(TW3DCompiledShader(RTDTemporalAccumulation_PixelByteCode), "SVGFTemporalAccumulation");
	svgf_wf_s = new TW3DShader(TW3DCompiledShader(RTDWaveletFilter_PixelByteCode), "SVGFWaveletFilter");


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
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0, D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		false, false, false, false
		);

	rt_ps = new TW3DComputePipelineState(rs);
	rt_ps->SetShader(rt_s);
	rt_ps->Create(device);


	rs = new TW3DRootSignature(device,
		{
			TW3DRPTexture(SVGFTA_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFTA_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFTA_FILTERED_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_filt_direct"s)),
			TW3DRPTexture(SVGFTA_FILTERED_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_filt_indirect"s)),
			TW3DRPTexture(SVGFTA_MOTION, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_motion"s)),
			TW3DRPTexture(SVGFTA_COMPACT_DATA, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_compact_data"s)),
			TW3DRPTexture(SVGFTA_PREV_COMPACT_DATA, D3D12_SHADER_VISIBILITY_PIXEL, svgf_ta_s->GetRegister("g_prev_compact_data"s)),
		},
		true, true, false, false
		);

	svgf_ta_ps = new TW3DGraphicsPipelineState(rs);
	svgf_ta_ps->SetRTVFormat(0, TWT::RGBA32Float);
	svgf_ta_ps->SetRTVFormat(1, TWT::RGBA32Float);
	//svgf_ta_ps->SetRTVFormat(2, TWT::RGBA32Float);
	//svgf_ta_ps->SetRTVFormat(3, TWT::R16Float);
	svgf_ta_ps->SetVertexShader(sq_s);
	svgf_ta_ps->SetPixelShader(svgf_ta_s);
	svgf_ta_ps->Create(device);


	rs = new TW3DRootSignature(device,
		{
			TW3DRPTexture(SVGFWF_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFWF_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFWF_PREV_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_prev_direct"s)),
			TW3DRPTexture(SVGFWF_PREV_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_prev_indirect"s)),
			TW3DRPTexture(SVGFWF_DETAIL_SUM_DIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_detail_sum_direct"s), true),
			TW3DRPTexture(SVGFWF_DETAIL_SUM_INDIRECT, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_detail_sum_indirect"s), true),
			TW3DRPTexture(SVGFWF_COMPACT_DATA, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("g_compact_data"s)),
			TW3DRPConstants(SVGFWF_INPUT_DATA, D3D12_SHADER_VISIBILITY_PIXEL, svgf_wf_s->GetRegister("input_data"s), 4)
		},
		true, true, false, false
	);

	svgf_wf_ps = new TW3DGraphicsPipelineState(rs);
	svgf_wf_ps->SetRTVFormat(0, TWT::RGBA32Float);
	svgf_wf_ps->SetRTVFormat(1, TWT::RGBA32Float);
	svgf_wf_ps->SetVertexShader(sq_s);
	svgf_wf_ps->SetPixelShader(svgf_wf_s);
	svgf_wf_ps->Create(device);


	direct_tex = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	indirect_tex = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	direct_albedo_tex = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	indirect_albedo_tex = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	direct_tex->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	indirect_tex->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	direct_albedo_tex->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	indirect_albedo_tex->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	direct_out = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float);
	indirect_out = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float);

	//svgf_linear_z_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float);
	svgf_mo_vec_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA16Float);
	svgf_compact_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float, TWT::float4(1000));
	svgf_prev_compact_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float, TWT::float4(1000));

	//svgf_prev_linear_z_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float);

	detail_sum_direct = ResourceManager->CreateTexture2D(GSize, TWT::RGBA32Float, true);
	detail_sum_direct->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	detail_sum_indirect = ResourceManager->CreateTexture2D(GSize, TWT::RGBA32Float, true);
	detail_sum_indirect->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	for (TWT::uint i = 0; i < 2; i++) {
		svgf_swap_fb[i] = ResourceManager->CreateFramebuffer(GSize);
		svgf_swap_fb[i]->AddRenderTarget(0, TWT::RGBA32Float, TWT::float4(0));
		svgf_swap_fb[i]->AddRenderTarget(1, TWT::RGBA32Float, TWT::float4(0));
	}

	svgf_filtered_fb = ResourceManager->CreateFramebuffer(GSize);
	svgf_filtered_fb->AddRenderTarget(0, TWT::RGBA32Float, TWT::float4(0));
	svgf_filtered_fb->AddRenderTarget(1, TWT::RGBA32Float, TWT::float4(0));

	/*svgf_ta_curr_fb = ResourceManager->CreateFramebuffer(RTSize);
	svgf_ta_curr_fb->AddRenderTarget(0, TWT::RGBA32Float);
	svgf_ta_curr_fb->AddRenderTarget(1, TWT::RGBA32Float);
	svgf_ta_curr_fb->AddRenderTarget(2, TWT::RGBA32Float);
	svgf_ta_curr_fb->AddRenderTarget(3, TWT::R16Float);

	svgf_ta_prev_fb = ResourceManager->CreateFramebuffer(RTSize);
	svgf_ta_prev_fb->AddRenderTarget(0, TWT::RGBA32Float);
	svgf_ta_prev_fb->AddRenderTarget(1, TWT::RGBA32Float);
	svgf_ta_prev_fb->AddRenderTarget(2, TWT::RGBA32Float);
	svgf_ta_prev_fb->AddRenderTarget(3, TWT::R16Float);*/
}

TW3DRaytracer::~TW3DRaytracer() {
	delete sq_s;
	delete rt_s;
	delete svgf_ta_s;
	delete svgf_wf_s;
	delete rt_ps;
	delete svgf_ta_ps;
	delete svgf_wf_ps;

	delete direct_tex;
	delete indirect_tex;
	delete direct_albedo_tex;
	delete indirect_albedo_tex;
	delete direct_out;
	delete indirect_out;

	//delete svgf_linear_z_rt;
	delete svgf_mo_vec_rt;
	delete svgf_compact_rt;
	delete svgf_prev_compact_rt;

	//delete svgf_prev_linear_z_rt;

	delete detail_sum_direct;
	delete detail_sum_indirect;

	for (TWT::uint i = 0; i < 2; i++)
		delete svgf_swap_fb[i];
	delete svgf_filtered_fb;
	//delete svgf_ta_curr_fb;
	//delete svgf_ta_prev_fb;
}

void TW3DRaytracer::Resize(TWT::uint2 GSize, TWT::uint2 RTSize) {

	//svgf_linear_z_rt->Resize(GSize);
	svgf_mo_vec_rt->Resize(GSize);
	svgf_compact_rt->Resize(GSize);
	svgf_prev_compact_rt->Resize(GSize);
	//svgf_prev_linear_z_rt->Resize(GSize);

	direct_out->Resize(GSize);
	indirect_out->Resize(GSize);
	direct_tex->Resize(RTSize);
	direct_albedo_tex->Resize(RTSize);
	indirect_tex->Resize(RTSize);
	indirect_albedo_tex->Resize(RTSize);

	detail_sum_direct->Resize(GSize);
	detail_sum_indirect->Resize(GSize);


	for (TWT::uint i = 0; i < 2; i++)
		svgf_swap_fb[i]->Resize(GSize);

	svgf_filtered_fb->Resize(GSize);

	/*svgf_ta_curr_fb->Resize(RTSize);
	svgf_ta_prev_fb->Resize(RTSize);*/
}

void TW3DRaytracer::TraceRays(TW3DGraphicsCommandList* CL,
		TW3DRenderTarget* GPosition, TW3DRenderTarget* GDiffuse, TW3DRenderTarget* GSpecular, TW3DRenderTarget* GNormal, TW3DRenderTarget* GEmission, TW3DTexture* GDepth,
		TW3DTexture* DiffuseTexArr, TW3DTexture* EmissionTexArr, TW3DTexture* NormalTexArr, TW3DConstantBuffer* RendererInfoCB,
		TW3DScene* Scene, TW3DScene* LargeScaleScene) {

	CL->SetPipelineState(rt_ps);
	Scene->Bind(CL, RT_GVB_BUFFER, RT_SCENE_BUFFER, RT_GNB_BUFFER, RT_LSB_BUFFER);

	if (LargeScaleScene)
		LargeScaleScene->Bind(CL, RT_L_GVB_BUFFER, RT_L_SCENE_BUFFER, RT_L_GNB_BUFFER, RT_L_LSB_BUFFER);

	CL->BindTexture(RT_G_POSITION_TEXTURE, GPosition);
	CL->BindTexture(RT_G_DIFFUSE_TEXTURE, GDiffuse);
	CL->BindTexture(RT_G_SPECULAR_TEXTURE, GSpecular);
	CL->BindTexture(RT_G_EMISSION_TEXTURE, GEmission);
	CL->BindTexture(RT_G_NORMAL_TEXTURE, GNormal);

	CL->BindTexture(RT_DIFFUSE_TEXTURE, DiffuseTexArr);
	//CL->BindTexture(RT_SPECULAR_TEXTURE, specular_texarr);
	CL->BindTexture(RT_EMISSION_TEXTURE, EmissionTexArr);
	CL->BindTexture(RT_NORMAL_TEXTURE, NormalTexArr);

	CL->BindTexture(RT_DIRECT_TEXTURE, direct_tex, true);
	CL->BindTexture(RT_DIRECT_ALBEDO_TEXTURE, direct_albedo_tex, true);
	CL->BindTexture(RT_INDIRECT_TEXTURE, indirect_tex, true);
	CL->BindTexture(RT_INDIRECT_ALBEDO_TEXTURE, indirect_albedo_tex, true);

	CL->BindConstantBuffer(RT_CAMERA_CB, Scene->Camera->GetConstantBuffer());
	CL->BindUIntConstant(RT_INPUT_CONST, 0, 0);
	CL->BindUIntConstant(RT_INPUT_CONST, Scene->LightSources.size(), 1);

	if (LargeScaleScene)
		CL->BindUIntConstant(RT_INPUT_CONST, LargeScaleScene->LightSources.size(), 2);

	CL->BindConstantBuffer(RT_RENDERERINFO_CB, RendererInfoCB);
	CL->Dispatch(ceil(TWT::float2(size) / 8.0f));
	CL->ResourceBarrier(TW3DUAVBarrier());
}

void TW3DRaytracer::DenoiseResult(TW3DGraphicsCommandList* CL) {
	// SVGF Temporal Accululation
	CL->SetViewportScissor(svgf_filtered_fb->GetSize());

	CL->SetPipelineState(svgf_ta_ps);

	CL->ResourceBarriers({
		TW3DTransitionBarrier(svgf_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(svgf_mo_vec_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(svgf_prev_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		});

	CL->BindTexture(SVGFTA_DIRECT, direct_tex);
	CL->BindTexture(SVGFTA_INDIRECT, indirect_tex);
	CL->BindTexture(SVGFTA_FILTERED_DIRECT, svgf_filtered_fb->GetRenderTarget(0));
	CL->BindTexture(SVGFTA_FILTERED_INDIRECT, svgf_filtered_fb->GetRenderTarget(1));
	CL->BindTexture(SVGFTA_MOTION, svgf_mo_vec_rt);
	CL->BindTexture(SVGFTA_COMPACT_DATA, svgf_compact_rt);
	CL->BindTexture(SVGFTA_PREV_COMPACT_DATA, svgf_prev_compact_rt);

	CL->BindFramebuffer(svgf_swap_fb[1]);
	CL->DrawQuad();

	CL->ResourceBarriers({
		TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
		TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(1), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
		TW3DTransitionBarrier(svgf_mo_vec_rt, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
		TW3DTransitionBarrier(svgf_prev_compact_rt, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)
	});


	// SVGF Wavelet filtering
	CL->ClearTexture(detail_sum_direct, TWT::float4(0));
	CL->ClearTexture(detail_sum_indirect, TWT::float4(0));

	const TWT::uint max_iterations = 4;

	for (TWT::uint i = 0; i < max_iterations; i++) {
		CL->ResourceBarriers({
			TW3DTransitionBarrier(svgf_swap_fb[1]->GetRenderTarget(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(svgf_swap_fb[1]->GetRenderTarget(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		});

		CL->SetPipelineState(svgf_wf_ps);
		CL->BindTexture(SVGFWF_DIRECT, svgf_swap_fb[1]->GetRenderTarget(0));
		CL->BindTexture(SVGFWF_INDIRECT, svgf_swap_fb[1]->GetRenderTarget(1));
		CL->BindTexture(SVGFWF_PREV_DIRECT, svgf_filtered_fb->GetRenderTarget(0));
		CL->BindTexture(SVGFWF_PREV_INDIRECT, svgf_filtered_fb->GetRenderTarget(1));
		CL->BindTexture(SVGFWF_DETAIL_SUM_DIRECT, detail_sum_direct, true);
		CL->BindTexture(SVGFWF_DETAIL_SUM_INDIRECT, detail_sum_indirect, true);
		CL->BindTexture(SVGFWF_COMPACT_DATA, svgf_compact_rt);
		CL->BindUIntConstant(SVGFWF_INPUT_DATA, i, 0);
		CL->BindUIntConstant(SVGFWF_INPUT_DATA, max_iterations, 1);
		TWT::float2 g_scale = TWT::float2(direct_out->GetSize()) / TWT::float2(direct_tex->GetSize());
		CL->BindFloatConstant(SVGFWF_INPUT_DATA, g_scale.x, 2);
		CL->BindFloatConstant(SVGFWF_INPUT_DATA, g_scale.y, 3);

		CL->BindFramebuffer(svgf_swap_fb[0]);
		CL->DrawQuad();

		CL->ResourceBarriers({
			TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
			TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(1), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
			TW3DTransitionBarrier(svgf_swap_fb[1]->GetRenderTarget(0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
			TW3DTransitionBarrier(svgf_swap_fb[1]->GetRenderTarget(1), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
			TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE),
			TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE)
		});

		CL->CopyTextureRegion(svgf_filtered_fb->GetRenderTarget(0), svgf_swap_fb[0]->GetRenderTarget(0));
		CL->CopyTextureRegion(svgf_filtered_fb->GetRenderTarget(1), svgf_swap_fb[0]->GetRenderTarget(1));

		CL->ResourceBarriers({
			TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(0), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET),
			TW3DTransitionBarrier(svgf_filtered_fb->GetRenderTarget(1), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET),
			TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(0), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
			TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(1), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET)
		});

		std::swap(svgf_swap_fb[0], svgf_swap_fb[1]);
	}


	if (max_iterations % 2 == 0)
		std::swap(svgf_swap_fb[0], svgf_swap_fb[1]);

	/*CL->ResourceBarriers({
		TW3DTransitionBarrier(svgf_compact_rt, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE),
	    TW3DTransitionBarrier(svgf_prev_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST)
	});*/

	//CL->CopyTextureRegion(svgf_prev_compact_rt, svgf_compact_rt);

	CL->ResourceBarriers({
		TW3DTransitionBarrier(svgf_compact_rt, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	    //TW3DTransitionBarrier(svgf_prev_compact_rt, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET),
	    TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(0), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE),
	    TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(1), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE),
	    TW3DTransitionBarrier(direct_out, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST),
	    TW3DTransitionBarrier(indirect_out, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST)
	});

	std::swap(svgf_compact_rt, svgf_prev_compact_rt);

	// Output the result of SVGF to the expected output buffer for subsequent passes.
	CL->CopyTextureRegion(direct_out, svgf_swap_fb[0]->GetRenderTarget(0));
	CL->CopyTextureRegion(indirect_out, svgf_swap_fb[0]->GetRenderTarget(1));

	CL->ResourceBarriers({
		TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(0), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	    TW3DTransitionBarrier(svgf_swap_fb[0]->GetRenderTarget(1), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET),
	    TW3DTransitionBarrier(direct_out, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET),
	    TW3DTransitionBarrier(indirect_out, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET)
	});
}
