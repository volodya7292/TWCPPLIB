#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"
#include "TW3DGraphicsPipelineState.h"
#include "TW3DBitonicSorter.h"

#include "CompiledShaders/GBuffer.v.h"
#include "CompiledShaders/GBuffer.p.h"
#include "CompiledShaders/ScreenQuad.v.h"
#include "CompiledShaders/FinalPassBlit.p.h"
#include "CompiledShaders/RayTrace.c.h"


TW3DDefaultRenderer::~TW3DDefaultRenderer() {
	delete gbuffer_ps;
	delete blit_ps;

	delete rt_cl;
	delete g_cl;
	delete gd_cl;

	delete info_cb;

	delete g_position;
	delete g_normal;
	delete g_diffuse;
	delete g_specular;
	delete g_emission;
	delete g_depth;

	delete diffuse_texarr;
	delete specular_texarr;
	delete emission_texarr;
	delete normal_texarr;

	delete ray_tracer;
}

void TW3DDefaultRenderer::CreateBlitResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature(Device,
		{
			TW3DRPTexture(0, D3D12_SHADER_VISIBILITY_PIXEL, 0), // GBuffer diffuse
			TW3DRPTexture(1, D3D12_SHADER_VISIBILITY_PIXEL, 1), // GBuffer emission
			TW3DRPTexture(2, D3D12_SHADER_VISIBILITY_PIXEL, 2), // RT direct
			TW3DRPTexture(3, D3D12_SHADER_VISIBILITY_PIXEL, 3), // RT direct albedo
			TW3DRPTexture(4, D3D12_SHADER_VISIBILITY_PIXEL, 4), // RT indirect
			TW3DRPTexture(5, D3D12_SHADER_VISIBILITY_PIXEL, 5), // RT indirect albedo
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		true, true, false, false
	);

	D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthDesc.DepthEnable = false;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	blit_ps = new TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		root_signature);
	blit_ps->SetRTVFormat(0, TWT::RGBA8Unorm);
	blit_ps->SetVertexShader(new TW3DShader(TW3DCompiledShader(ScreenQuad_VertexByteCode), "ScreenQuad"s));
	blit_ps->SetPixelShader(new TW3DShader(TW3DCompiledShader(FinalPassBlit_PixelByteCode), "BlitPixel"s));
	blit_ps->Create(Device);
}

void TW3DDefaultRenderer::CreateGBufferResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature(Device,
		{
			TW3DRPConstantBuffer(GBUFFER_VERTEX_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, 0),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_PREV_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, 1),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_VMI_CB, D3D12_SHADER_VISIBILITY_VERTEX, 2),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_CAMERA_CB, D3D12_SHADER_VISIBILITY_PIXEL, 0),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, D3D12_SHADER_VISIBILITY_PIXEL, 1),
			TW3DRPTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 0),
			TW3DRPTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 1),
			TW3DRPTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 2),
			TW3DRPTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 3),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) }
	);

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({ TW3D_ILE_POSITION, TW3D_ILE_TEXCOORD_3D, TW3D_ILE_NORMAL, TW3D_ILE_TANGENT, TW3D_ILE_BITANGENT });

	D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	gbuffer_ps = new TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		root_signature);
	gbuffer_ps->SetRTVFormat(0, TWT::RGBA32Float); //-V525
	gbuffer_ps->SetRTVFormat(1, TWT::RGBA32Float);
	gbuffer_ps->SetRTVFormat(2, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(3, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(4, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(5, TWT::RGBA16Float);
	gbuffer_ps->SetRTVFormat(6, TWT::RGBA32Float);
	gbuffer_ps->SetVertexShader(new TW3DShader(TW3DCompiledShader(GBuffer_VertexByteCode), "GBufferVertex"s));
	gbuffer_ps->SetPixelShader(new TW3DShader(TW3DCompiledShader(GBuffer_PixelByteCode), "GBufferPixel"s));
	gbuffer_ps->SetInputLayout(input_layout);
	gbuffer_ps->Create(Device);

	g_position = ResourceManager->CreateRenderTarget(TWT::uint2(Width, Height), TWT::RGBA32Float, TWT::float4(0));
	g_normal = ResourceManager->CreateRenderTarget(TWT::uint2(Width, Height), TWT::RGBA32Float);
	g_diffuse = ResourceManager->CreateRenderTarget(TWT::uint2(Width, Height), TWT::RGBA8Unorm);
	g_specular = ResourceManager->CreateRenderTarget(TWT::uint2(Width, Height), TWT::RGBA8Unorm);
	g_emission = ResourceManager->CreateRenderTarget(TWT::uint2(Width, Height), TWT::RGBA8Unorm);
	g_depth = ResourceManager->CreateDepthStencilTexture(TWT::uint2(Width, Height));

	g_cl = ResourceManager->CreateDirectCommandList();
	gd_cl = ResourceManager->CreateDirectCommandList();
}

void TW3DDefaultRenderer::CreateRTResources() {
	rt_cl = ResourceManager->CreateComputeCommandList();
	ray_tracer = new TW3DRaytracer(ResourceManager, TWT::uint2(Width, Height), TWT::uint2(Width, Height));
}

void TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Width, Height);
	TWU::TW3DLogInfo("TW3DRenderer initialized."s);
	CreateBlitResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] BlitResources initialized."s);
	CreateGBufferResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] GBufferResources initialized."s);
	CreateRTResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] RTResources initialized."s);

	info.info = TWT::uint4(Width, Height, 0, 0);
	info_cb = ResourceManager->CreateConstantBuffer(1, sizeof(TWT::DefaultRendererInfoCB));

	diffuse_texarr = ResourceManager->CreateTextureArray2D(TWT::uint2(400, 400), material_count, TWT::RGBA8Unorm);
	specular_texarr = ResourceManager->CreateTextureArray2D(TWT::uint2(400, 400), material_count, TWT::RGBA8Unorm);
	emission_texarr = ResourceManager->CreateTextureArray2D(TWT::uint2(400, 400), material_count, TWT::RGBA8Unorm);
	normal_texarr = ResourceManager->CreateTextureArray2D(TWT::uint2(400, 400), material_count, TWT::RGBA32Float);
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] Texture resources initialized."s);

	//diffuse_texarr->Upload2D(L"D:/OptimizedRT_converged.jpg", 0);
	diffuse_texarr->Upload2D(L"D:/emission.png", 0);
	emission_texarr->Upload2D(L"D:/emission.png", 1);
	//texture->Upload2D(L"D:/тест2.png", 1);
	//texture = ResourceManager->CreateTexture2D(L"D:/тест.png");
}

void TW3DDefaultRenderer::Resize(TWT::uint Width, TWT::uint Height) {
	TW3DRenderer::Resize(Width, Height);

	info.info.x = Width;
	info.info.y = Height;

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height));
	scissor = CD3DX12_RECT(0, 0, Width, Height);

	TWT::uint2 rt_size = TWT::float2(Width, Height) * rt_scale;

	g_position->Resize(TWT::uint2(Width, Height));
	g_normal->Resize(TWT::uint2(Width, Height));
	g_diffuse->Resize(TWT::uint2(Width, Height));
	g_specular->Resize(TWT::uint2(Width, Height));
	g_emission->Resize(TWT::uint2(Width, Height));
	g_depth->Resize(TWT::uint2(Width, Height));

	ray_tracer->Resize(TWT::uint2(Width, Height), rt_size);
}

void TW3DDefaultRenderer::BlitOutput(TW3DGraphicsCommandList* cl, TW3DRenderTarget* ColorOutput, TW3DTexture* Depth) {
	cl->ResourceBarriers({
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
		TW3DTransitionBarrier(ray_tracer->direct_tex, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(ray_tracer->direct_albedo_tex, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(ray_tracer->indirect_tex, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
		TW3DTransitionBarrier(ray_tracer->indirect_albedo_tex, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	});

	cl->SetPipelineState(blit_ps);
	cl->SetRenderTarget(ColorOutput, Depth);
	cl->BindTexture(0, g_diffuse);
	cl->BindTexture(1, g_emission);
	cl->BindTexture(2, ray_tracer->direct_out);
	cl->BindTexture(3, ray_tracer->direct_albedo_tex);
	cl->BindTexture(4, ray_tracer->indirect_out);
	cl->BindTexture(5, ray_tracer->indirect_albedo_tex);
	cl->ClearRTV(ColorOutput, TWT::float4(0, 0, 0, 1));
	cl->ClearDSVDepth(Depth);
	cl->SetViewport(&viewport);
	cl->SetScissor(&scissor);
	cl->DrawQuad();

	cl->ResourceBarriers({
		TW3DTransitionBarrier(ray_tracer->direct_tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3DTransitionBarrier(ray_tracer->direct_albedo_tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3DTransitionBarrier(ray_tracer->indirect_tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3DTransitionBarrier(ray_tracer->indirect_albedo_tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
	});
}

void TW3DDefaultRenderer::RenderRecordGBuffer() {
	g_cl->Reset();
	g_cl->BindResources(ResourceManager);
	g_cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_cl->SetPipelineState(gbuffer_ps);

	g_cl->ResourceBarrier(ray_tracer->svgf_mo_vec_rt, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	g_cl->ResourceBarrier(ray_tracer->svgf_compact_rt, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	g_cl->ResourceBarrier(ray_tracer->svgf_prev_compact_rt, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	g_cl->SetRenderTargets({ g_position, g_normal, g_diffuse, g_specular, g_emission, ray_tracer->svgf_mo_vec_rt, ray_tracer->svgf_compact_rt }, g_depth);
	g_cl->ClearRTV(ray_tracer->svgf_compact_rt);
	g_cl->ClearRTV(g_emission);
	g_cl->ClearRTV(g_position);
	g_cl->ClearRTV(g_diffuse);
	g_cl->ClearDSVDepth(g_depth);
	g_cl->SetViewport(&viewport);
	g_cl->SetScissor(&scissor);

	g_cl->BindCameraCBV(GBUFFER_VERTEX_CAMERA_CB, Scene->Camera);
	g_cl->BindCameraPrevCBV(GBUFFER_VERTEX_PREV_CAMERA_CB, Scene->Camera);
	g_cl->BindCameraCBV(GBUFFER_PIXEL_CAMERA_CB, Scene->Camera);
	g_cl->BindConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, info_cb);
	g_cl->BindTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, diffuse_texarr);
	g_cl->BindTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, specular_texarr);
	g_cl->BindTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, emission_texarr);
	g_cl->BindTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, normal_texarr);

	for (TW3DObject* object : Scene->Objects)
		g_cl->DrawObject(object, GBUFFER_VERTEX_VMI_CB);

	g_cl->ResourceBarrier(ray_tracer->svgf_mo_vec_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	g_cl->ResourceBarrier(ray_tracer->svgf_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	g_cl->ResourceBarrier(ray_tracer->svgf_prev_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	g_cl->Close();

	ResourceManager->ExecuteCommandList(g_cl);
	ResourceManager->FlushCommandList(g_cl);
}

void TW3DDefaultRenderer::Record(TWT::uint BackBufferIndex, TW3DRenderTarget* ColorOutput, TW3DTexture* DepthStencilOutput) {
	TW3DRenderer::Record(BackBufferIndex, ColorOutput, DepthStencilOutput);

	
	record_cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	BlitOutput(record_cl, ColorOutput, DepthStencilOutput);
	record_cl->Close();
}

void TW3DDefaultRenderer::RecordBeforeExecution() {
	Scene->RecordBeforeExecution();

	// Trace rays
	// -------------------------------------------------------------------------------------------------------------------------
	rt_cl->Reset();
	rt_cl->BindResources(ResourceManager);

	ray_tracer->TraceRays(rt_cl, g_position, g_diffuse, g_specular, g_normal, g_emission, g_depth, diffuse_texarr, emission_texarr, normal_texarr, info_cb, Scene, LargeScaleScene);

	ray_tracer->DenoiseResult(rt_cl);

	rt_cl->Close();

	/*gd_cl->Reset();
	gd_cl->BindResources(ResourceManager);

	ray_tracer->DenoiseResult(gd_cl);

	gd_cl->Close();*/
}

void TW3DDefaultRenderer::Update(float DeltaTime) {
	Scene->Update(DeltaTime);
	info_cb->Update(&info);
}

void TW3DDefaultRenderer::Execute(TWT::uint BackBufferIndex) {
	TW3DRenderer::Execute(BackBufferIndex);

	RenderRecordGBuffer();

	ResourceManager->ExecuteCommandList(rt_cl);
	ResourceManager->FlushCommandList(rt_cl);
	//ResourceManager->ExecuteCommandList(gd_cl);
	//ResourceManager->FlushCommandList(gd_cl);

	ResourceManager->ExecuteCommandList(execute_cl);

	// Adjust frame count
	info.info.z++;
}