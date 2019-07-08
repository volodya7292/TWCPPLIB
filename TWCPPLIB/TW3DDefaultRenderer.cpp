#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"

#ifdef _DEBUG
#include "CompiledShaders/Debug/GBuffer.v.h"
#include "CompiledShaders/Debug/GBuffer.g.h"
#include "CompiledShaders/Debug/GBuffer.p.h"
#include "CompiledShaders/Debug/ScreenQuad.v.h"
#else
#include "CompiledShaders/Release/GBuffer.v.h"
#include "CompiledShaders/Release/GBuffer.g.h"
#include "CompiledShaders/Release/GBuffer.p.h"
#include "CompiledShaders/Release/ScreenQuad.v.h"
#endif // _DEBUG


TW3DDefaultRenderer::~TW3DDefaultRenderer() {

}

void TW3DDefaultRenderer::CreateGBufferResources() {
	g_vertex_s = ResourceManager->RequestShader("GBufferVertex"s, TW3DCompiledShader(GBuffer_VertexByteCode));
	g_geom_s = ResourceManager->RequestShader("GBufferGeometry"s, TW3DCompiledShader(GBuffer_GeometryByteCode));
	g_pixel_s = ResourceManager->RequestShader("GBufferPixel"s, TW3DCompiledShader(GBuffer_PixelByteCode));

	TW3DRootSignature* root_signature = ResourceManager->RequestRootSignature("gbuffer_sign"s,
		{
			TW3DRPConstantBuffer(GBUFFER_VERTEX_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, g_vertex_s->GetRegister("camera"s)),
			//TW3DRPConstantBuffer(GBUFFER_VERTEX_PREV_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, g_vertex_s->GetRegister("prev_camera"s)),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_RENDERER_CB, D3D12_SHADER_VISIBILITY_VERTEX,  g_vertex_s->GetRegister("renderer"s)),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_VMI_CB, D3D12_SHADER_VISIBILITY_VERTEX, g_vertex_s->GetRegister("vertex_mesh"s)),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_CAMERA_CB, D3D12_SHADER_VISIBILITY_PIXEL,  g_pixel_s->GetRegister("camera"s)),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, D3D12_SHADER_VISIBILITY_PIXEL,  g_pixel_s->GetRegister("renderer"s)),
			TW3DRPTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("diffuse_tex"s)),
			TW3DRPTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("specular_tex"s)),
			TW3DRPTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("emission_tex"s)),
			TW3DRPTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("normal_tex"s)),
			TW3DRPBuffer(GBUFFER_PIXEL_MATERIAL_BUFFER, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("materials"s)),
			TW3DRPConstantBuffer(GBUFFER_GEOMETRY_MATRICES, D3D12_SHADER_VISIBILITY_GEOMETRY, g_geom_s->GetRegister("input_data"s)),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		true, true, true
		);

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({ TW3D_ILE_POSITION, TW3D_ILE_TEXCOORD_3D, TW3D_ILE_NORMAL, TW3D_ILE_TANGENT });

	D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	gbuffer_ps = ResourceManager->RequestGraphicsPipeline("GBufferPipeline"s,
		root_signature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		{ TWT::RGBA32Float, TWT::RGBA16Float, TWT::RGBA8Unorm, TWT::RGBA8Unorm, TWT::RGBA8Unorm }, g_vertex_s, g_geom_s, g_pixel_s, input_layout
	);

	g_position = ResourceManager->RequestRenderTargetCube("g_position"s, Width, TWT::RGBA32Float, TWT::float4(0));
	g_normal = ResourceManager->RequestRenderTargetCube("g_normal"s, Width, TWT::RGBA16Float);
	g_diffuse = ResourceManager->RequestRenderTargetCube("g_diffuse"s, Width, TWT::RGBA8Unorm);
	g_specular = ResourceManager->RequestRenderTargetCube("g_specular"s, Width, TWT::RGBA8Unorm);
	g_emission = ResourceManager->RequestRenderTargetCube("g_emission"s, Width, TWT::RGBA8Unorm);
	g_depth = ResourceManager->RequestDepthStencilCubeTexture("g_depth_tex"s, Width, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE, D3D12_DSV_FLAG_READ_ONLY_DEPTH);

	//objs_cmd_buffer = ResourceManager->RequestCommandBuffer(""s, )

	material_buffer = ResourceManager->RequestBuffer("material_buffer"s, 8192, sizeof(TWT::DefaultMaterial));
	cb_camera_matrices = ResourceManager->RequestConstantBuffer("cb_camera_matrices"s, 1, sizeof(CameraCubeMatrices));
}

void TW3DDefaultRenderer::CreateRTResources() {

}

void TW3DDefaultRenderer::record_g_buffer_objects(TW3DCommandList* cl) {
	cl->SetRootSignatureFrom(gbuffer_ps);
	cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cl->BindCameraCBV(GBUFFER_VERTEX_CAMERA_CB, Scene->Camera);
	//cl->BindCameraPrevCBV(GBUFFER_VERTEX_PREV_CAMERA_CB, Scene->Camera);
	cl->BindCameraCBV(GBUFFER_PIXEL_CAMERA_CB, Scene->Camera);

	for (TW3DObject* object : Scene->Objects)
		cl->DrawObject(object, GBUFFER_VERTEX_VMI_CB);

	if (LargeScaleScene) {
		cl->BindCameraCBV(GBUFFER_VERTEX_CAMERA_CB, LargeScaleScene->Camera);
		//cl->BindCameraPrevCBV(GBUFFER_VERTEX_PREV_CAMERA_CB, LargeScaleScene->Camera);
		cl->BindCameraCBV(GBUFFER_PIXEL_CAMERA_CB, LargeScaleScene->Camera);

		for (TW3DObject* object : LargeScaleScene->Objects)
			cl->DrawObject(object, GBUFFER_VERTEX_VMI_CB);
	}
}

void TW3DDefaultRenderer::record_g_buffer(TW3DSCFrame* frame, TW3DCommandList* cl) {
	cl->Reset();
	cl->BindResources(ResourceManager);


	cl->SetRootSignatureFrom(gbuffer_ps);
	cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	cl->SetRenderTargets({ g_position, g_normal, g_diffuse, g_specular, g_emission }, g_depth);

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
	cl->BindBuffer(GBUFFER_PIXEL_MATERIAL_BUFFER, material_buffer);

	cl->BindConstantBuffer(GBUFFER_VERTEX_RENDERER_CB, info_cb);
	cl->BindConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, info_cb);
	cl->BindConstantBuffer(GBUFFER_GEOMETRY_MATRICES, cb_camera_matrices);

	cl->ExecuteBundle(frame->GetCommandList("GBufferObjects"s));


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

	g_position->Resize(TWT::uint2(Width, Height));
	g_normal->Resize(TWT::uint2(Width, Height));
	g_diffuse->Resize(TWT::uint2(Width, Height));
	g_specular->Resize(TWT::uint2(Width, Height));
	g_emission->Resize(TWT::uint2(Width, Height));
	g_depth->Resize(TWT::uint2(Width, Height));
}

void TW3DDefaultRenderer::BlitOutput(TW3DCommandList* cl, TW3DRenderTarget* ColorOutput) {
	cl->Reset();
	cl->BindResources(ResourceManager);

	cl->ResourceBarriers({
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST),
	});

	//cl->CopyTextureRegion(ColorOutput, ray_tracer->Output);

	cl->ResourceBarriers({
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT),
	});

	cl->Close();
}

void TW3DDefaultRenderer::Update(float DeltaTime) {
	Scene->Update(DeltaTime);
	if (LargeScaleScene)
		LargeScaleScene->Update(DeltaTime);

	info_cb->Update(&info);
}

void TW3DDefaultRenderer::Execute(TW3DSCFrame* Frame) {
	TW3DRenderer::Execute(Frame);

	Scene->BeforeExecution();
	if (LargeScaleScene)
		LargeScaleScene->BeforeExecution();


	auto g_cl = Frame->GetCommandList("GBuffer");
	record_g_buffer(Frame, g_cl);

	Frame->ExecuteCommandList("GBuffer"s, false, true);

	Frame->ExecuteCommandList("RayTrace"s, false, true);
	Frame->ExecuteCommandList("Blit"s, false, true);



	// Adjust frame count
	info.info.z++;
}