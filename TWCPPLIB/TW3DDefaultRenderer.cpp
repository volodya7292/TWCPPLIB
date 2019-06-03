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
	delete rt_ps;
	delete blit_ps;

	delete rt_cl;
	delete g_cl;
	
	delete info_cb;

	delete g_position;
	delete g_normal;
	delete g_diffuse;
	delete g_specular;
	delete g_emission;
	delete g_depth;

	delete rt_output;
	delete diffuse_texarr;
	delete specular_texarr;
	delete emission_texarr;
	delete normal_texarr;
}

void TW3DDefaultRenderer::CreateBlitResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature(Device,
		{
			TW3DRPTexture(0, D3D12_SHADER_VISIBILITY_PIXEL, 0), // GBuffer albedo
			TW3DRPTexture(1, D3D12_SHADER_VISIBILITY_PIXEL, 1)  // RT result
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
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
		root_signature,
		1);
	blit_ps->SetRTVFormat(0, TWT::RGBA8Unorm);
	blit_ps->SetVertexShader(new TW3DShader(TW3DCompiledShader(ScreenQuad_VertexByteCode), "ScreenQuad"s));
	blit_ps->SetPixelShader(new TW3DShader(TW3DCompiledShader(FinalPassBlit_PixelByteCode), "BlitPixel"s));
	blit_ps->Create(Device);
}

void TW3DDefaultRenderer::CreateGBufferResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature(Device,
		{
			TW3DRPConstantBuffer(GBUFFER_VERTEX_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, 0),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_VMI_CB, D3D12_SHADER_VISIBILITY_VERTEX, 1),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_CAMERA_CB, D3D12_SHADER_VISIBILITY_PIXEL, 0),
			TW3DRPConstants(GBUFFER_PIXEL_VMISCALE_CONST, D3D12_SHADER_VISIBILITY_PIXEL, 1, 1),
			TW3DRPTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 0),
			TW3DRPTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 1),
			TW3DRPTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 2),
			TW3DRPTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, 3),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) }
	);

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({TW3D_ILE_POSITION, TW3D_ILE_TEXCOORD_3D, TW3D_ILE_NORMAL, TW3D_ILE_TANGENT, TW3D_ILE_BITANGENT});

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
		root_signature,
		5);
	gbuffer_ps->SetRTVFormat(0, TWT::RGBA32Float);
	gbuffer_ps->SetRTVFormat(1, TWT::RGBA32Float);
	gbuffer_ps->SetRTVFormat(2, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(3, TWT::RGBA8Unorm);
	gbuffer_ps->SetRTVFormat(4, TWT::RGBA8Unorm);
	gbuffer_ps->SetVertexShader(new TW3DShader(TW3DCompiledShader(GBuffer_VertexByteCode), "GBufferVertex"s));
	gbuffer_ps->SetPixelShader(new TW3DShader(TW3DCompiledShader(GBuffer_PixelByteCode), "GBufferPixel"s));
	gbuffer_ps->SetInputLayout(input_layout);
	gbuffer_ps->Create(Device);


	g_position = ResourceManager->CreateRenderTarget(Width, Height, TWT::RGBA32Float, TWT::vec4(0));
	g_normal = ResourceManager->CreateRenderTarget(Width, Height, TWT::RGBA32Float);
	g_diffuse = ResourceManager->CreateRenderTarget(Width, Height, TWT::RGBA8Unorm);
	g_specular = ResourceManager->CreateRenderTarget(Width, Height, TWT::RGBA8Unorm);
	g_emission = ResourceManager->CreateRenderTarget(Width, Height, TWT::RGBA8Unorm);
	g_depth = ResourceManager->CreateDepthStencilTexture(Width, Height);

	g_cl = ResourceManager->CreateDirectCommandList();
}

void TW3DDefaultRenderer::CreateRTResources() {
	rt_output = ResourceManager->CreateTexture2D(Width, Height, TWT::RGBA8Unorm, true);
	ResourceManager->ResourceBarrier(rt_output, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	TW3DShader* s = new TW3DShader(TW3DCompiledShader(RayTrace_ByteCode), "RTShader"s);

	TW3DRootSignature* rs = new TW3DRootSignature(Device,
		{
			TW3DRPBuffer(RT_GVB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("gvb"s)),
			TW3DRPBuffer(RT_SCENE_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("scene"s)),
			TW3DRPBuffer(RT_GNB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("gnb"s)),
			TW3DRPBuffer(RT_LSB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("lsb"s)),
			TW3DRPBuffer(RT_L_GVB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("l_gvb"s)),
			TW3DRPBuffer(RT_L_SCENE_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("l_scene"s)),
			TW3DRPBuffer(RT_L_GNB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("l_gnb"s)),
			TW3DRPBuffer(RT_L_LSB_BUFFER, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("l_lsb"s)),
			
			TW3DRPTexture(RT_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("diffuse_tex"s)),
			TW3DRPTexture(RT_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("specular_tex"s)),
			TW3DRPTexture(RT_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("emission_tex"s)),
			TW3DRPTexture(RT_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("normal_tex"s)),

			TW3DRPTexture(RT_G_POSITION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("g_position"s)),
			TW3DRPTexture(RT_G_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("g_normal"s)),
			TW3DRPTexture(RT_G_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("g_diffuse"s)),
			TW3DRPTexture(RT_G_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("g_specular"s)),
			TW3DRPTexture(RT_G_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("g_emission"s)),

			TW3DRPTexture(RT_OUTPUT_TEXTURE, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("rt_output"s), true),
			TW3DRPConstantBuffer(RT_CAMERA_CB, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("camera"s)),
			TW3DRPConstants(RT_INPUT_CONST, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("input"s), 3),
			TW3DRPConstantBuffer(RT_RENDERERINFO_CB, D3D12_SHADER_VISIBILITY_ALL, s->GetRegister("renderer"s)),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		false, false, false, false
	);

	rt_ps = new TW3DComputePipelineState(rs);

	rt_ps->SetShader(s);
	rt_ps->Create(Device);

	rt_cl = ResourceManager->CreateComputeCommandList();
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

	info.info.x = 0;
	info_cb = ResourceManager->CreateConstantBuffer(1, sizeof(TWT::DefaultRendererInfoCB));

	diffuse_texarr = ResourceManager->CreateTextureArray2D(400, 400, material_count, TWT::RGBA8Unorm);
	specular_texarr = ResourceManager->CreateTextureArray2D(400, 400, material_count, TWT::RGBA8Unorm);
	emission_texarr = ResourceManager->CreateTextureArray2D(400, 400, material_count, TWT::RGBA8Unorm);
	normal_texarr = ResourceManager->CreateTextureArray2D(400, 400, material_count, TWT::RGBA32Float);
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] Texture resources initialized."s);

	//diffuse_texarr->Upload2D(L"D:/OptimizedRT.png", 0);
	diffuse_texarr->Upload2D(L"D:/emission.png", 0);
	emission_texarr->Upload2D(L"D:/emission.png", 1);
	//texture->Upload2D(L"D:/тест2.png", 1);
	//texture = ResourceManager->CreateTexture2D(L"D:/тест.png");
}

void TW3DDefaultRenderer::Resize(TWT::uint Width, TWT::uint Height) {
	TW3DRenderer::Resize(Width, Height);

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height));
	scissor = CD3DX12_RECT(0, 0, Width, Height);

	rt_output->InitialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	rt_output->Resize(Width, Height);

	g_position->Resize(Width, Height);
	g_normal->Resize(Width, Height);
	g_diffuse->Resize(Width, Height);
	g_specular->Resize(Width, Height);
	g_emission->Resize(Width, Height);
	g_depth->Resize(Width, Height);
}

void TW3DDefaultRenderer::BlitOutput(TW3DGraphicsCommandList* cl, TW3DRenderTarget* ColorOutput, TW3DTexture* Depth) {
	cl->ResourceBarriers({
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
		TW3DTransitionBarrier(rt_output, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	});

	cl->SetPipelineState(blit_ps);
	cl->SetRenderTarget(ColorOutput, Depth);
	cl->BindTexture(0, g_diffuse);
	cl->BindTexture(1, rt_output);
	cl->ClearRTV(ColorOutput, TWT::vec4(0, 0, 0, 1));
	cl->ClearDSVDepth(Depth);
	cl->SetViewport(&viewport);
	cl->SetScissor(&scissor);
	cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cl->Draw(4);

	cl->ResourceBarriers({
		TW3DTransitionBarrier(rt_output, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
	});
}

void TW3DDefaultRenderer::RenderRecordGBuffer() {
	g_cl->Reset();
	g_cl->BindResources(ResourceManager);
	g_cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_cl->SetPipelineState(gbuffer_ps);

	g_cl->SetRenderTargets({g_position, g_normal, g_diffuse, g_specular, g_emission}, g_depth);
	g_cl->ClearRTV(g_position);
	g_cl->ClearRTV(g_diffuse);
	g_cl->ClearDSVDepth(g_depth);
	g_cl->SetViewport(&viewport);
	g_cl->SetScissor(&scissor);

	g_cl->BindCameraCBV(GBUFFER_VERTEX_CAMERA_CB, Scene->Camera);
	g_cl->BindCameraCBV(GBUFFER_PIXEL_CAMERA_CB, Scene->Camera);
	g_cl->BindTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, diffuse_texarr);
	g_cl->BindTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, specular_texarr);
	g_cl->BindTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, emission_texarr);
	g_cl->BindTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, normal_texarr);

	for (TW3DObject* object : Scene->Objects)
		g_cl->DrawObject(object, GBUFFER_VERTEX_VMI_CB);

	g_cl->Close();

	ResourceManager->ExecuteCommandList(g_cl);
	ResourceManager->FlushCommandList(g_cl);
}

void TW3DDefaultRenderer::Record(TWT::uint BackBufferIndex, TW3DRenderTarget* ColorOutput, TW3DTexture* DepthStencilOutput) {
	TW3DRenderer::Record(BackBufferIndex, ColorOutput, DepthStencilOutput);

	record_cl->Reset();
	record_cl->BindResources(ResourceManager);
	record_cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	BlitOutput(record_cl, ColorOutput, DepthStencilOutput);
	record_cl->Close();
}

void TW3DDefaultRenderer::RecordBeforeExecution() {
	Scene->RecordBeforeExecution();

	// Trace rays
	// -------------------------------------------------------------------------------------------------------------------------
	//ResourceManager->FlushCommandList(rt_cl);
	rt_cl->Reset();
	rt_cl->BindResources(ResourceManager);
	rt_cl->SetPipelineState(rt_ps);
	Scene->Bind(rt_cl, RT_GVB_BUFFER, RT_SCENE_BUFFER, RT_GNB_BUFFER, RT_LSB_BUFFER);

	if (LargeScaleScene)
		LargeScaleScene->Bind(rt_cl, RT_L_GVB_BUFFER, RT_L_SCENE_BUFFER, RT_L_GNB_BUFFER, RT_L_LSB_BUFFER);

	rt_cl->BindTexture(RT_G_POSITION_TEXTURE, g_position);
	rt_cl->BindTexture(RT_G_NORMAL_TEXTURE, g_normal);
	rt_cl->BindTexture(RT_G_DIFFUSE_TEXTURE, g_diffuse);
	rt_cl->BindTexture(RT_G_SPECULAR_TEXTURE, g_specular);
	rt_cl->BindTexture(RT_G_EMISSION_TEXTURE, g_emission);

	rt_cl->BindTexture(RT_DIFFUSE_TEXTURE, diffuse_texarr);
	rt_cl->BindTexture(RT_SPECULAR_TEXTURE, specular_texarr);
	rt_cl->BindTexture(RT_EMISSION_TEXTURE, emission_texarr);
	rt_cl->BindTexture(RT_NORMAL_TEXTURE, normal_texarr);

	rt_cl->BindTexture(RT_OUTPUT_TEXTURE, rt_output, true);
	rt_cl->BindConstantBuffer(RT_CAMERA_CB, Scene->Camera->GetConstantBuffer());
	rt_cl->Bind32BitConstant(RT_INPUT_CONST, 0, 0);
	rt_cl->Bind32BitConstant(RT_INPUT_CONST, Scene->LightSources.size(), 1);
	
	if (LargeScaleScene)
		rt_cl->Bind32BitConstant(RT_INPUT_CONST, LargeScaleScene->LightSources.size(), 2);

	rt_cl->BindConstantBuffer(RT_RENDERERINFO_CB, info_cb);
	rt_cl->Dispatch(ceil(Width / 8.0f), ceil(Height / 8.0f));
	rt_cl->ResourceBarrier(TW3DUAVBarrier());
	rt_cl->Close();
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

	ResourceManager->ExecuteCommandList(execute_cl);

	info.info.x++;
}