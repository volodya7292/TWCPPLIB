#include "pch.h"
#include "TW3DRaytracer.h"
#include "TW3DScene.h"
#include "CompiledShaders/RayTrace.c.h"
#include "CompiledShaders/ScreenQuad.v.h"
#include "CompiledShaders/RTDTemporalAccumulation.c.h"
#include "CompiledShaders/RTDWaveletFilter.c.h"

TW3DRaytracer::TW3DRaytracer(TW3DResourceManager* ResourceManager, TWT::uint2 GSize, TWT::uint2 RTSize) :
	size(RTSize) {
	auto device = ResourceManager->GetDevice();

	sq_s = new TW3DShader(TW3DCompiledShader(ScreenQuad_VertexByteCode), "ScreenQuad");
	rt_s = new TW3DShader(TW3DCompiledShader(RayTrace_ByteCode), "RayTrace");
	svgf_ta_s = new TW3DShader(TW3DCompiledShader(RTDTemporalAccumulation_ByteCode), "SVGFTemporalAccumulation");
	svgf_wf_s = new TW3DShader(TW3DCompiledShader(RTDWaveletFilter_ByteCode), "SVGFWaveletFilter");


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
			//TW3DRPTexture(RT_G_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("g_emission"s)),

			// Ray tracing output resources
			TW3DRPTexture(RT_DIRECT_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_direct"s), true),
			TW3DRPTexture(RT_INDIRECT_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_indirect"s), true),
			//TW3DRPTexture(RT_G_DATA_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, rt_s->GetRegister("rt_g_data"s), true),

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
			TW3DRPTexture(SVGFTA_DIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFTA_INDIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFTA_FILTERED_DIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_filt_direct"s)),
			TW3DRPTexture(SVGFTA_FILTERED_INDIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_filt_indirect"s)),
			TW3DRPTexture(SVGFTA_MOTION, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_motion"s)),
			TW3DRPTexture(SVGFTA_COMPACT_DATA, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_compact_data"s)),
			TW3DRPTexture(SVGFTA_PREV_COMPACT_DATA, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_prev_compact_data"s)),
			TW3DRPTexture(SVGFTA_DIRECT_OUT, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_direct_out"s), true),
			TW3DRPTexture(SVGFTA_INDIRECT_OUT, D3D12_SHADER_VISIBILITY_ALL, svgf_ta_s->GetRegister("g_indirect_out"s), true),
		}
	);

	svgf_ta_ps = new TW3DComputePipelineState(rs);
	svgf_ta_ps->SetShader(svgf_ta_s);
	svgf_ta_ps->Create(device);


	rs = new TW3DRootSignature(device,
		{
			TW3DRPTexture(SVGFWF_DIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_direct"s)),
			TW3DRPTexture(SVGFWF_INDIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_indirect"s)),
			TW3DRPTexture(SVGFWF_PREV_DIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_prev_direct"s)),
			TW3DRPTexture(SVGFWF_PREV_INDIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_prev_indirect"s)),
			TW3DRPTexture(SVGFWF_DETAIL_SUM_DIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_detail_sum_direct"s), true),
			TW3DRPTexture(SVGFWF_DETAIL_SUM_INDIRECT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_detail_sum_indirect"s), true),
			TW3DRPTexture(SVGFWF_DIRECT_OUT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_direct_out"s), true),
			TW3DRPTexture(SVGFWF_INDIRECT_OUT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_indirect_out"s), true),
			TW3DRPTexture(SVGFWF_DIRECT_FINAL_OUT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_direct_final_out"s), true),
			TW3DRPTexture(SVGFWF_INDIRECT_FINAL_OUT, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_indirect_final_out"s), true),
			TW3DRPTexture(SVGFWF_COMPACT_DATA, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("g_compact_data"s)),
			TW3DRPConstants(SVGFWF_INPUT_DATA, D3D12_SHADER_VISIBILITY_ALL, svgf_wf_s->GetRegister("input"s), 2)
		}
	);

	svgf_wf_ps = new TW3DComputePipelineState(rs);
	svgf_wf_ps->SetShader(svgf_wf_s);
	svgf_wf_ps->Create(device);


	direct_in = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	indirect_in = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32UInt, true); // RGBA32UInt
	direct_in->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	indirect_in->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	direct_in->Resize(direct_in->GetSize());
	indirect_in->Resize(direct_in->GetSize());

	direct_out = ResourceManager->CreateTexture2D(GSize, TWT::RGBA32Float, true);
	indirect_out = ResourceManager->CreateTexture2D(GSize, TWT::RGBA32Float, true);
	direct_out->InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	indirect_out->InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	direct_out->Resize(direct_out->GetSize());
	indirect_out->Resize(direct_out->GetSize());

	temporal_history = ResourceManager->CreateTexture2D(RTSize, TWT::R32UInt, true);
	temporal_history->InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	temporal_history->Resize(temporal_history->GetSize());

	//svgf_linear_z_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float);
	svgf_mo_vec_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA16Float);
	svgf_compact_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float, TWT::float4(1000));
	svgf_prev_compact_rt = ResourceManager->CreateRenderTarget(GSize, TWT::RGBA32Float, TWT::float4(1000));
	svgf_mo_vec_rt->InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	svgf_compact_rt->InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	svgf_prev_compact_rt->InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
	svgf_mo_vec_rt->Resize(svgf_mo_vec_rt->GetSize());
	svgf_compact_rt->Resize(svgf_compact_rt->GetSize());
	svgf_prev_compact_rt->Resize(svgf_prev_compact_rt->GetSize());


	detail_sum_direct = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	detail_sum_direct->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	detail_sum_indirect = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	detail_sum_indirect->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

	for (TWT::uint i = 0; i < 2; i++) {
		svgf_swap_direct[i] = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
		svgf_swap_indirect[i] = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32UInt, true); // RGBA32UInt
		svgf_swap_direct[i]->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		svgf_swap_indirect[i]->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		svgf_swap_direct[i]->Resize(svgf_swap_direct[i]->GetSize());
		svgf_swap_indirect[i]->Resize(svgf_swap_indirect[i]->GetSize());
	}

	svgf_filtered_direct = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32Float, true);
	svgf_filtered_indirect = ResourceManager->CreateTexture2D(RTSize, TWT::RGBA32UInt, true); // RGBA32UInt
	svgf_filtered_direct->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	svgf_filtered_indirect->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	svgf_filtered_direct->Resize(svgf_filtered_direct->GetSize());
	svgf_filtered_indirect->Resize(svgf_filtered_indirect->GetSize());
}

TW3DRaytracer::~TW3DRaytracer() {
	delete sq_s;
	delete rt_s;
	delete svgf_ta_s;
	delete svgf_wf_s;
	delete rt_ps;
	delete svgf_ta_ps;
	delete svgf_wf_ps;

	delete direct_in;
	delete indirect_in;
	delete direct_out;
	delete indirect_out;

	delete svgf_mo_vec_rt;
	delete svgf_compact_rt;
	delete svgf_prev_compact_rt;

	delete temporal_history;

	delete detail_sum_direct;
	delete detail_sum_indirect;

	for (TWT::uint i = 0; i < 2; i++) {
		//delete svgf_swap_fb[i];
		delete svgf_swap_direct[i];
		delete svgf_swap_indirect[i];
	}

	delete svgf_filtered_direct;
	delete svgf_filtered_indirect;
}

void TW3DRaytracer::Resize(TWT::uint2 GSize, TWT::uint2 RTSize) {

	//svgf_linear_z_rt->Resize(GSize);
	svgf_mo_vec_rt->Resize(GSize);
	svgf_compact_rt->Resize(GSize);
	svgf_prev_compact_rt->Resize(GSize);
	//svgf_prev_linear_z_rt->Resize(GSize);

	direct_in->Resize(RTSize);
	indirect_in->Resize(RTSize);
	direct_out->Resize(GSize);
	indirect_out->Resize(GSize);

	detail_sum_direct->Resize(RTSize);
	detail_sum_indirect->Resize(RTSize);


	for (TWT::uint i = 0; i < 2; i++) {
		//svgf_swap_fb[i]->Resize(GSize);
		svgf_swap_direct[i]->Resize(RTSize);
		svgf_swap_indirect[i]->Resize(RTSize);
	}

	temporal_history->Resize(RTSize);

	//svgf_filtered_fb->Resize(GSize);
	svgf_filtered_direct->Resize(RTSize);
	svgf_filtered_indirect->Resize(RTSize);

	/*svgf_ta_curr_fb->Resize(RTSize);
	svgf_ta_prev_fb->Resize(RTSize);*/
}

void TW3DRaytracer::TraceRays(TW3DCommandList* CL,
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
	//CL->BindTexture(RT_G_EMISSION_TEXTURE, GEmission);
	CL->BindTexture(RT_G_NORMAL_TEXTURE, GNormal);

	CL->BindTexture(RT_DIFFUSE_TEXTURE, DiffuseTexArr);
	//CL->BindTexture(RT_SPECULAR_TEXTURE, specular_texarr);
	CL->BindTexture(RT_EMISSION_TEXTURE, EmissionTexArr);
	CL->BindTexture(RT_NORMAL_TEXTURE, NormalTexArr);

	CL->BindTexture(RT_DIRECT_TEXTURE, direct_in, true);
	//CL->BindTexture(RT_DIRECT_ALBEDO_TEXTURE, direct_albedo_tex, true);
	CL->BindTexture(RT_INDIRECT_TEXTURE, indirect_in, true);
	//CL->BindTexture(RT_INDIRECT_ALBEDO_TEXTURE, indirect_albedo_tex, true);

	CL->BindConstantBuffer(RT_CAMERA_CB, Scene->Camera->GetConstantBuffer());
	CL->BindUIntConstant(RT_INPUT_CONST, 0, 0);
	CL->BindUIntConstant(RT_INPUT_CONST, Scene->LightSources.size(), 1);

	if (LargeScaleScene)
		CL->BindUIntConstant(RT_INPUT_CONST, LargeScaleScene->LightSources.size(), 2);

	CL->BindConstantBuffer(RT_RENDERERINFO_CB, RendererInfoCB);
	CL->Dispatch(ceil(TWT::float2(size) / 8.0f));
	CL->ResourceBarrier(TW3DUAVBarrier());
}

void TW3DRaytracer::DenoiseResult(TW3DCommandList* CL) {

	// SVGF Temporal Accululation
	// -----------------------------------------------------------------------------------------------------------------------------------------------------

	CL->SetPipelineState(svgf_ta_ps);

	CL->ResourceBarriers({
		TW3DTransitionBarrier(svgf_filtered_direct, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(svgf_filtered_indirect, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
	});

	CL->BindTexture(SVGFTA_DIRECT, direct_in);
	CL->BindTexture(SVGFTA_INDIRECT, indirect_in);
	CL->BindTexture(SVGFTA_FILTERED_DIRECT, svgf_filtered_direct);
	CL->BindTexture(SVGFTA_FILTERED_INDIRECT, svgf_filtered_indirect);
	CL->BindTexture(SVGFTA_MOTION, svgf_mo_vec_rt);
	CL->BindTexture(SVGFTA_COMPACT_DATA, svgf_compact_rt);
	CL->BindTexture(SVGFTA_PREV_COMPACT_DATA, svgf_prev_compact_rt);
	CL->BindTexture(SVGFTA_DIRECT_OUT, svgf_swap_direct[1], true);
	CL->BindTexture(SVGFTA_INDIRECT_OUT, svgf_swap_indirect[1], true);

	CL->Dispatch(ceil(TWT::float2(size) / 8.0f));


	// SVGF Temporal Accululation
	// -----------------------------------------------------------------------------------------------------------------------------------------------------

	CL->ClearTexture(detail_sum_direct, TWT::float4(0));
	CL->ClearTexture(detail_sum_indirect, TWT::float4(0));

	CL->SetPipelineState(svgf_wf_ps);
	CL->BindTexture(SVGFWF_PREV_DIRECT, svgf_filtered_direct);
	CL->BindTexture(SVGFWF_PREV_INDIRECT, svgf_filtered_indirect);
	CL->BindTexture(SVGFWF_DETAIL_SUM_DIRECT, detail_sum_direct, true);
	CL->BindTexture(SVGFWF_DETAIL_SUM_INDIRECT, detail_sum_indirect, true);
	CL->BindTexture(SVGFWF_COMPACT_DATA, svgf_compact_rt);

	for (TWT::uint i = 0; i < SPATIAL_FILTER_MAX_ITERATIONS; i++) {
		CL->ResourceBarriers({
			TW3DTransitionBarrier(svgf_swap_direct[1], D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(svgf_swap_indirect[1], D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(indirect_out, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		});

		CL->BindTexture(SVGFWF_DIRECT, svgf_swap_direct[1]);
		CL->BindTexture(SVGFWF_INDIRECT, svgf_swap_indirect[1]);
		CL->BindUIntConstant(SVGFWF_INPUT_DATA, i, 0);
		CL->BindUIntConstant(SVGFWF_INPUT_DATA, SPATIAL_FILTER_MAX_ITERATIONS, 1);

		CL->BindTexture(SVGFWF_DIRECT_OUT, svgf_swap_direct[0], true);
		CL->BindTexture(SVGFWF_INDIRECT_OUT, svgf_swap_indirect[0], true);

		// Dispatch
		if (i == SPATIAL_FILTER_MAX_ITERATIONS - 1) {
			CL->BindTexture(SVGFWF_DIRECT_FINAL_OUT, direct_out, true);
			CL->BindTexture(SVGFWF_INDIRECT_FINAL_OUT, indirect_out, true);
			CL->Dispatch(ceil(TWT::float2(direct_out->GetSize()) / 8.0f));
		} else {
			CL->Dispatch(ceil(TWT::float2(size) / 8.0f));
		}

		CL->ResourceBarriers({
			TW3DTransitionBarrier(indirect_out, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(svgf_filtered_direct, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
			TW3DTransitionBarrier(svgf_filtered_indirect, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST),
			TW3DTransitionBarrier(svgf_swap_direct[1], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
			TW3DTransitionBarrier(svgf_swap_indirect[1], D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
			TW3DTransitionBarrier(svgf_swap_direct[0], D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE),
			TW3DTransitionBarrier(svgf_swap_indirect[0], D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE)
		});

		// Copy spatially filtered image
		CL->CopyTextureRegion(svgf_filtered_direct, svgf_swap_direct[0]);
		CL->CopyTextureRegion(svgf_filtered_indirect, svgf_swap_indirect[0]);

		if (i == SPATIAL_FILTER_MAX_ITERATIONS - 1) {
			CL->ResourceBarriers({
				TW3DTransitionBarrier(svgf_filtered_direct, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
				TW3DTransitionBarrier(svgf_filtered_indirect, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
				TW3DTransitionBarrier(svgf_swap_direct[0], D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
				TW3DTransitionBarrier(svgf_swap_indirect[0], D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
			});
		} else {
			CL->ResourceBarriers({
				TW3DTransitionBarrier(svgf_filtered_direct, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
				TW3DTransitionBarrier(svgf_filtered_indirect, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
				TW3DTransitionBarrier(svgf_swap_direct[0], D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
				TW3DTransitionBarrier(svgf_swap_indirect[0], D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
			});
		}

		std::swap(svgf_swap_direct[0], svgf_swap_direct[1]);
		std::swap(svgf_swap_indirect[0], svgf_swap_indirect[1]);
	}


	if (SPATIAL_FILTER_MAX_ITERATIONS % 2 == 0) {
		std::swap(svgf_swap_direct[0], svgf_swap_direct[1]);
		std::swap(svgf_swap_indirect[0], svgf_swap_indirect[1]);
	}

	std::swap(svgf_compact_rt, svgf_prev_compact_rt);
}
