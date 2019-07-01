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

	delete ray_tracer;
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
	gbuffer_ps->SetRTVFormat(0, TWT::RGBA32Float);
	gbuffer_ps->SetRTVFormat(1, TWT::RGBA32Float);
	gbuffer_ps->SetRTVFormat(2, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(3, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(4, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(5, TWT::RG16Float);
	gbuffer_ps->SetRTVFormat(6, TWT::RGBA32Float);
	gbuffer_ps->SetVertexShader(new TW3DShader(TW3DCompiledShader(GBuffer_VertexByteCode), "GBufferVertex"s));
	gbuffer_ps->SetPixelShader(new TW3DShader(TW3DCompiledShader(GBuffer_PixelByteCode), "GBufferPixel"s));
	gbuffer_ps->SetInputLayout(input_layout);
	gbuffer_ps->Create(Device);

	g_position = ResourceManager->RequestRenderTarget("g_position"s, TWT::uint2(Width, Height), TWT::RGBA32Float, TWT::float4(0));
	g_normal = ResourceManager->RequestRenderTarget("g_normal"s, TWT::uint2(Width, Height), TWT::RGBA32Float);
	g_diffuse = ResourceManager->RequestRenderTarget("g_diffuse"s, TWT::uint2(Width, Height), TWT::RGBA8Unorm);
	g_specular = ResourceManager->RequestRenderTarget("g_specular"s, TWT::uint2(Width, Height), TWT::RGBA8Unorm);
	g_emission = ResourceManager->RequestRenderTarget("g_emission"s, TWT::uint2(Width, Height), TWT::RGBA8Unorm);
	g_depth = ResourceManager->RequestDepthStencilTexture("g_depth_tex"s, TWT::uint2(Width, Height));
}

void TW3DDefaultRenderer::CreateRTResources() {
	ray_tracer = new TW3DRaytracer(ResourceManager, TWT::uint2(Width, Height), TWT::uint2(Width, Height));
}

void TW3DDefaultRenderer::record_g_buffer_objects(TW3DCommandList* cl) {
	cl->SetRootSignatureFrom(gbuffer_ps);
	cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (TW3DObject* object : Scene->Objects)
		cl->DrawObject(object, GBUFFER_VERTEX_VMI_CB);
}

void TW3DDefaultRenderer::record_g_buffer(TW3DSCFrame* frame, TW3DCommandList* cl) {
	cl->Reset();
	cl->BindResources(ResourceManager);
	

	cl->SetRootSignatureFrom(gbuffer_ps);
	cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cl->ResourceBarrier(ray_tracer->svgf_mo_vec_rt, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cl->ResourceBarrier(ray_tracer->svgf_compact_rt, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	cl->ResourceBarrier(ray_tracer->svgf_prev_compact_rt, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cl->SetRenderTargets({ g_position, g_normal, g_diffuse, g_specular, g_emission, ray_tracer->svgf_mo_vec_rt, ray_tracer->svgf_compact_rt }, g_depth);
	cl->ClearRTV(ray_tracer->svgf_compact_rt);

	cl->ClearRTV(g_emission);
	cl->ClearRTV(g_position);
	cl->ClearRTV(g_diffuse);
	cl->ClearDSVDepth(g_depth);
	cl->SetViewport(&viewport);
	cl->SetScissor(&scissor);

	cl->BindTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, diffuse_texarr);
	cl->BindTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, specular_texarr);
	cl->BindTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, emission_texarr);
	cl->BindTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, normal_texarr);

	cl->BindCameraCBV(GBUFFER_VERTEX_CAMERA_CB, Scene->Camera);
	cl->BindCameraPrevCBV(GBUFFER_VERTEX_PREV_CAMERA_CB, Scene->Camera);
	cl->BindCameraCBV(GBUFFER_PIXEL_CAMERA_CB, Scene->Camera);
	cl->BindConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, info_cb);

	cl->ExecuteBundle(frame->GetCommandList("GBufferObjects"s));
	
	cl->ResourceBarrier(ray_tracer->svgf_mo_vec_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	cl->ResourceBarrier(ray_tracer->svgf_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	cl->ResourceBarrier(ray_tracer->svgf_prev_compact_rt, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);


	cl->Close();
}

void TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Width, Height);
	TWU::TW3DLogInfo("TW3DRenderer initialized."s);
	CreateGBufferResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] GBufferResources initialized."s);
	CreateRTResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] RTResources initialized."s);

	info.info = TWT::uint4(Width, Height, 0, 0);
	info_cb = ResourceManager->RequestConstantBuffer("render_info_cb"s, 1, sizeof(TWT::DefaultRendererInfoCB));

	diffuse_texarr = ResourceManager->RequestTextureArray2D("diffuse_arr"s, TWT::uint2(400, 400), material_count, TWT::RGBA8Unorm);
	specular_texarr = ResourceManager->RequestTextureArray2D("specular_arr"s, TWT::uint2(400, 400), material_count, TWT::RGBA8Unorm);
	emission_texarr = ResourceManager->RequestTextureArray2D("emission_arr"s, TWT::uint2(400, 400), material_count, TWT::RGBA8Unorm);
	normal_texarr = ResourceManager->RequestTextureArray2D("normal_arr"s, TWT::uint2(400, 400), material_count, TWT::RGBA32Float);
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] Texture resources initialized."s);

	diffuse_texarr->Upload2D(L"D:/OptimizedRT_converged.png", 0);
	//diffuse_texarr->Upload2D(L"D:/emission.png", 0);
	//diffuse_texarr->Upload2D(L"D:/emission.png", 1);
	emission_texarr->Upload2D(L"D:/emission.png", 1);
	//texture->Upload2D(L"D:/тест2.png", 1);
	//texture = ResourceManager->CreateTexture2D(L"D:/тест.png");
}


void TW3DDefaultRenderer::InitializeFrame(TW3DSCFrame* Frame) {
	Frame->RequestCommandList("GBufferObjects"s, TW3D_CL_BUNDLE, gbuffer_ps, [&](TW3DSCFrame* Frame, TW3DCommandList* CommandList) { record_g_buffer_objects(CommandList); });

	auto g_cl = Frame->RequestCommandList("GBuffer"s, TW3D_CL_DIRECT, gbuffer_ps);

	auto blit_cl = Frame->RequestCommandList("Blit"s, TW3D_CL_DIRECT);
	BlitOutput(blit_cl, Frame->RenderTarget);
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

void TW3DDefaultRenderer::BlitOutput(TW3DCommandList* cl, TW3DRenderTarget* ColorOutput) {
	cl->Reset();
	cl->BindResources(ResourceManager);

	cl->ResourceBarriers({
	    TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST),
	    TW3DTransitionBarrier(ray_tracer->Output, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE)
	});

	cl->CopyTextureRegion(ColorOutput, ray_tracer->Output);

	cl->ResourceBarriers({
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT),
		TW3DTransitionBarrier(ray_tracer->Output, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	});

	cl->Close();
}

void TW3DDefaultRenderer::Update(float DeltaTime) {
	Scene->Update(DeltaTime);
	info_cb->Update(&info);
}

void TW3DDefaultRenderer::Execute(TW3DSCFrame* Frame) {
	TW3DRenderer::Execute(Frame);

	Scene->BeforeExecution();

	auto g_cl = Frame->GetCommandList("GBuffer");
	record_g_buffer(Frame, g_cl);

	Frame->ExecuteCommandList("GBuffer"s, false, true);

	auto rt_cl = Frame->RequestCommandList("RayTrace"s, TW3D_CL_COMPUTE);
	rt_cl->Reset();
	rt_cl->BindResources(ResourceManager);
	ray_tracer->Render(rt_cl, g_position, g_diffuse, g_specular, g_normal, g_emission, g_depth, diffuse_texarr, emission_texarr, normal_texarr, info_cb, Scene, LargeScaleScene);
	ray_tracer->Tonemap(rt_cl);
	rt_cl->Close();

	Frame->ExecuteCommandList("RayTrace"s, false, true);
	Frame->ExecuteCommandList("Blit"s, false, true);

	

	// Adjust frame count
	info.info.z++;
}