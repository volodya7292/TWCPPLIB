#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"

#ifdef _DEBUG
#include "CompiledShaders/Debug/GBufferCube.v.h"
#include "CompiledShaders/Debug/GBufferCube.g.h"
#include "CompiledShaders/Debug/GBufferFront.v.h"
#include "CompiledShaders/Debug/GBuffer.p.h"
#include "CompiledShaders/Debug/ScreenQuad.v.h"
#else
#include "CompiledShaders/Release/GBufferCube.v.h"
#include "CompiledShaders/Release/GBufferCube.g.h"
#include "CompiledShaders/Release/GBufferFront.v.h"
#include "CompiledShaders/Release/GBuffer.p.h"
#include "CompiledShaders/Release/ScreenQuad.v.h"
#endif // _DEBUG

TW3DDefaultRenderer::~TW3DDefaultRenderer() {

}

void TW3DDefaultRenderer::CreateGBufferResources() {
	auto g_cube_vertex_s = ResourceManager->RequestShader("GBufferCubeVertex"s, TW3DCompiledShader(GBufferCube_VertexByteCode));
	auto g_cube_geom_s = ResourceManager->RequestShader("GBufferCubeGeometry"s, TW3DCompiledShader(GBufferCube_GeometryByteCode));
	auto g_front_vertex_s = ResourceManager->RequestShader("GBufferFrontVertex"s, TW3DCompiledShader(GBufferFront_VertexByteCode));
	auto g_pixel_s = ResourceManager->RequestShader("GBufferPixel"s, TW3DCompiledShader(GBuffer_PixelByteCode));

	TW3DRootSignature* root_signature = ResourceManager->RequestRootSignature("gbuffer_sign"s,
		{
			TW3DRPConstantBuffer(GBUFFER_VERTEX_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, g_cube_vertex_s->GetRegister("camera"s)),
			//TW3DRPConstantBuffer(GBUFFER_VERTEX_PREV_CAMERA_CB, D3D12_SHADER_VISIBILITY_VERTEX, g_vertex_s->GetRegister("prev_camera"s)),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_RENDERER_CB, D3D12_SHADER_VISIBILITY_VERTEX,  g_cube_vertex_s->GetRegister("renderer"s)),
			TW3DRPConstantBuffer(GBUFFER_VERTEX_VMI_CB, D3D12_SHADER_VISIBILITY_VERTEX, g_cube_vertex_s->GetRegister("vertex_mesh"s)),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_CAMERA_CB, D3D12_SHADER_VISIBILITY_PIXEL,  g_pixel_s->GetRegister("camera"s)),
			TW3DRPConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, D3D12_SHADER_VISIBILITY_PIXEL,  g_pixel_s->GetRegister("renderer"s)),
			TW3DRPTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("diffuse_tex"s)),
			TW3DRPTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("specular_tex"s)),
			TW3DRPTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("emission_tex"s)),
			TW3DRPTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("normal_tex"s)),
			TW3DRPBuffer(GBUFFER_PIXEL_MATERIAL_BUFFER, D3D12_SHADER_VISIBILITY_PIXEL, g_pixel_s->GetRegister("materials"s)),
			TW3DRPConstantBuffer(GBUFFER_GEOMETRY_MATRICES, D3D12_SHADER_VISIBILITY_GEOMETRY, g_cube_geom_s->GetRegister("input_data"s)),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_PIXEL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		true, true, true
		);

	std::vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({ TW3D_ILE_POSITION, TW3D_ILE_TEXCOORD_3D, TW3D_ILE_NORMAL, TW3D_ILE_TANGENT });

	D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	front_gbuffer_ps = ResourceManager->RequestGraphicsPipeline("FrontBufferPipeline"s,
		root_signature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		{ TWT::RGBA16Float, TWT::RGBA16Float, TWT::RGBA8Unorm, TWT::RGBA8Unorm, TWT::RGBA8Unorm }, g_front_vertex_s, g_pixel_s, input_layout
	);

	cube_gbuffer_ps = ResourceManager->RequestGraphicsPipeline("CubeGBufferPipeline"s,
		root_signature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		{ TWT::RGBA16Float, TWT::RGBA16Float, TWT::RGBA8Unorm, TWT::RGBA8Unorm, TWT::RGBA8Unorm }, g_cube_vertex_s, g_cube_geom_s, g_pixel_s, input_layout
	);

	front_gbuffer = ResourceManager->RequestFramebuffer("front_gbuffer", Size);
	front_gbuffer->AddRenderTarget(0, TWT::RGBA16Float, TWT::float4(0));
	front_gbuffer->AddRenderTarget(1, TWT::RGBA16Float);
	front_gbuffer->AddRenderTarget(2, TWT::RGBA8Unorm);
	front_gbuffer->AddRenderTarget(3, TWT::RGBA8Unorm);
	front_gbuffer->AddRenderTarget(4, TWT::RGBA8Unorm);
	front_gbuffer->AddDepthStencil();

	cube_gbuffer = ResourceManager->RequestFramebuffer("cube_gbuffer", TWT::uint2(Size.x * cube_gbuffer_scale), TW3D_FRAMEBUFFER_CUBE);
	cube_gbuffer->AddRenderTarget(0, TWT::RGBA16Float, TWT::float4(0));
	cube_gbuffer->AddRenderTarget(1, TWT::RGBA16Float);
	cube_gbuffer->AddRenderTarget(2, TWT::RGBA8Unorm);
	cube_gbuffer->AddRenderTarget(3, TWT::RGBA8Unorm);
	cube_gbuffer->AddRenderTarget(4, TWT::RGBA8Unorm);
	cube_gbuffer->AddDepthStencil();


	objs_cmd_sign = ResourceManager->RequestCommandSignature("objs_cmd_sign"s, {
		TW3DCAConstantBuffer(0, GBUFFER_VERTEX_VMI_CB),
		TW3DCAVertexBuffer(1),
		TW3DCADraw(2)
		}, root_signature);
	objs_cmd_buffer = ResourceManager->RequestCommandBuffer("objs_cmd_buffer"s, objs_cmd_sign, 16384, sizeof(TW3DScene::ObjectCmd));



	material_buffer = ResourceManager->RequestBuffer("material_buffer"s, 8192, sizeof(TWT::DefaultMaterial), false, true, D3D12_RESOURCE_STATE_COPY_DEST);
	cb_camera_matrices = ResourceManager->RequestConstantBuffer("cb_camera_matrices"s, 1, sizeof(CameraCubeMatrices));

	TWT::DefaultMaterial mat = {};
	mat.diffuse = TWT::float4(1);

	material_buffer->UpdateElement(&mat, 0);
}

void TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint2 Size) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Size);
	TWU::TW3DLogInfo("TW3DRenderer initialized."s);
	CreateGBufferResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] GBufferResources initialized."s);

	info.info = TWT::uint4(Size, 0, 1);
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
	{
		auto cl = Frame->RequestCommandList("GBuffer"s, TW3D_CL_DIRECT);

		cl->Reset();
		cl->BindResources(ResourceManager);

		cl->SetPipeline(front_gbuffer_ps);
		cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cl->BindFramebuffer(front_gbuffer);

		cl->ClearRTV(front_gbuffer->GetRenderTarget(0));
		cl->ClearDSVDepth(front_gbuffer->GetDepthStencilTexture());
		cl->SetViewportAndScissor(Size);

		cl->BindTexture(GBUFFER_PIXEL_DIFFUSE_TEXTURE, diffuse_texarr);
		cl->BindTexture(GBUFFER_PIXEL_SPECULAR_TEXTURE, specular_texarr);
		cl->BindTexture(GBUFFER_PIXEL_EMISSION_TEXTURE, emission_texarr);
		cl->BindTexture(GBUFFER_PIXEL_NORMAL_TEXTURE, normal_texarr);
		cl->BindBuffer(GBUFFER_PIXEL_MATERIAL_BUFFER, material_buffer);
		cl->BindConstantBuffer(GBUFFER_PIXEL_RENDERER_CB, info_cb);
		cl->BindCameraCBV(GBUFFER_PIXEL_CAMERA_CB, Scene->Camera);
		cl->BindConstantBuffer(GBUFFER_VERTEX_RENDERER_CB, info_cb);
		cl->BindCameraCBV(GBUFFER_VERTEX_CAMERA_CB, Scene->Camera);
		cl->BindConstantBuffer(GBUFFER_GEOMETRY_MATRICES, cb_camera_matrices);

		cl->DebugBeginEvent("Front GBuffer Rendering"s);
		cl->ExecuteIndirect(objs_cmd_buffer);
		cl->DebugEndEvent();


		cl->SetPipeline(cube_gbuffer_ps);

		cl->DebugBeginEvent("Cube GBuffer Rendering"s);
		cl->ExecuteIndirect(objs_cmd_buffer);
		cl->DebugEndEvent();

		cl->Close();
	}

	auto blit_cl = Frame->RequestCommandList("Blit"s, TW3D_CL_DIRECT);
	BlitOutput(blit_cl, Frame->RenderTarget);
}

void TW3DDefaultRenderer::Resize(TWT::uint2 Size) {
	TW3DRenderer::Resize(Size);

	info.info.x = Size.x;
	info.info.y = Size.y;

	front_gbuffer->Resize(Size);
	cube_gbuffer->Resize(TWT::uint2(Size.x));
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

	if (Scene->RenderCommandsUpdated) {
		Scene->RenderCommandsUpdated = false;

		std::vector<TW3DScene::ObjectCmd> cmd = Scene->GetObjectRenderCommands();
		objs_cmd_buffer->Update(cmd.data(), Scene->GetObjectRenderCommandCount());
	}

	info_cb->Update(&info);

	{
		TWT::float3 pos = Scene->Camera->Position;
		TWT::float4x4 proj_matrix = TWT::InfinitePerspective(90.0f, 1.0f); // Square perspection with 90 degrees FOV
	
		CameraCubeMatrices ccm;
		ccm.proj_view[0] = proj_matrix * TWT::ViewMatrix(pos, TWT::float3(0, -90, 0)); // left?
		ccm.proj_view[1] = proj_matrix * TWT::ViewMatrix(pos, TWT::float3(0, 0, 0));   // front
		ccm.proj_view[2] = proj_matrix * TWT::ViewMatrix(pos, TWT::float3(0, 90, 0));  // right?
		ccm.proj_view[3] = proj_matrix * TWT::ViewMatrix(pos, TWT::float3(0, 180, 0)); // back
		ccm.proj_view[4] = proj_matrix * TWT::ViewMatrix(pos, TWT::float3(-90, 0, 0)); // bottom
		ccm.proj_view[5] = proj_matrix * TWT::ViewMatrix(pos, TWT::float3(90, 0, 0));  // top

		cb_camera_matrices->Update(&ccm);
	}
}

void TW3DDefaultRenderer::Execute(TW3DSCFrame* Frame) {
	TW3DRenderer::Execute(Frame);

	Frame->ExecuteCommandList("GBuffer"s);

	// Adjust frame count
	info.info.z++;
}