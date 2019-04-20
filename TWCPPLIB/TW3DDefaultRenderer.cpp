#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"
#include "TW3DGraphicsPipelineState.h"

TW3D::TW3DResourceSR* texture;

TW3D::TW3DDefaultRenderer::~TW3DDefaultRenderer() {
	delete gbuffer_ps;
	delete gvb_ps;
	//delete blit_ps;
	delete texture;
	delete gvb;
}

void TW3D::TW3DDefaultRenderer::CreateGBufferResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature();
	root_signature->SetParameterCBV(0, D3D12_SHADER_VISIBILITY_VERTEX, 0); // Camera CB
	root_signature->SetParameterCBV(1, D3D12_SHADER_VISIBILITY_VERTEX, 1); // Vertex mesh CB
	root_signature->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_PIXEL, 0);  // Diffuse
	root_signature->SetParameterSRV(3, D3D12_SHADER_VISIBILITY_PIXEL, 1);  // Normal
	root_signature->SetParameterSRV(4, D3D12_SHADER_VISIBILITY_PIXEL, 2);  // Roughness
	root_signature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	root_signature->Create(Device);

	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({POSITION_ILE, TEXCOORD_ILE, NORMAL_ILE});

	D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	gbuffer_ps = new TW3D::TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		root_signature,
		1);
	gbuffer_ps->SetRTVFormat(0, TWT::RGBA8Unorm);
	gbuffer_ps->SetVertexShader("GBuffer.v.cso");
	gbuffer_ps->SetPixelShader("GBuffer.p.cso");
	gbuffer_ps->SetInputLayout(input_layout);
	gbuffer_ps->Create(Device);
}

void TW3D::TW3DDefaultRenderer::CreateGVBResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature(false, true, false);
	root_signature->SetParameterCBV(0, D3D12_SHADER_VISIBILITY_VERTEX, 0); // Vertex mesh CB
	root_signature->SetParameterUAV(1, D3D12_SHADER_VISIBILITY_VERTEX, 0); // Global Vertex Buffer UAV
	root_signature->Create(Device);

	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({ POSITION_ILE, TEXCOORD_ILE, NORMAL_ILE });

	gvb_ps = new TW3D::TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, root_signature);
	gvb_ps->SetVertexShader("GlobalVertexBuffer.v.cso");
	gvb_ps->SetInputLayout(input_layout);
	gvb_ps->Create(Device);

	gvb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(TWT::DefaultVertex));
}

void TW3D::TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Width, Height);
	CreateGBufferResources();
	CreateGVBResources();

	/*rastDesc.CullMode = D3D12_CULL_MODE_NONE;
	depthDesc.DepthEnable = FALSE;


	TW3DRootSignature* blitRootSignature = new TW3D::TW3DRootSignature(
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	blitRootSignature->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	blitRootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	blitRootSignature->Create(device);

	blit_ps = new TW3D::TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		blitRootSignature,
		1);
	blit_ps->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	blit_ps->SetVertexShader("VertexOffscreenBlit.cso");
	blit_ps->SetPixelShader("PixelOffscreenBlit.cso");
	blit_ps->Create(device);*/

	texture = ResourceManager->CreateTextureArray2D(720, 720, 10, DXGI_FORMAT_R8G8B8A8_UNORM);
	texture->Upload2D(L"D:/тест.png", 0);
	texture->Upload2D(L"D:/тест2.png", 1);
	//texture = ResourceManager->CreateTexture2D(L"D:/тест.png");
}

void TW3D::TW3DDefaultRenderer::Resize(TWT::UInt Width, TWT::UInt Height) {
	TW3DRenderer::Resize(Width, Height);

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<TWT::Float>(Width), static_cast<TWT::Float>(Height));
	scissor = CD3DX12_RECT(0, 0, Width, Height);
}

void TW3D::TW3DDefaultRenderer::Record(TWT::UInt BackBufferIndex, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStencilOutput) {
	TW3D::TW3DRenderer::Record(BackBufferIndex, ColorOutput, DepthStencilOutput);
	
	record_cl->Reset();
	record_cl->BindResources(ResourceManager);
	record_cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Global Vertex Buffer record
	// -------------------------------------------------------------------------------------------------------------------------
	record_cl->SetPipelineState(gvb_ps);
	record_cl->BindUAV(1, gvb);
	TWT::UInt VertexOffset = 0;
	gvb_vertex_buffers.clear();
	for (TW3DObject* object : Scene->objects)
		for (TW3DVertexBuffer* vb : object->VMInstance.VertexMesh->VertexBuffers)
			if (gvb_vertex_buffers.find(vb) == gvb_vertex_buffers.end()) {
				gvb_vertex_buffers.emplace(vb);

				vb->SetVertexOffset(VertexOffset);
				VertexOffset += vb->GetVertexCount();

				record_cl->SetVertexBuffer(0, vb->GetVBResource());
				record_cl->SetRootCBV(0, vb->GetCBResource(), 0);
				record_cl->Draw(vb->GetVertexCount());
			}
	gvb_vertex_count = VertexOffset + 1;


	// GBuffer record
	// -------------------------------------------------------------------------------------------------------------------------
	record_cl->SetPipelineState(gbuffer_ps);

	record_cl->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	record_cl->SetRenderTarget(ColorOutput, DepthStencilOutput);
	record_cl->ClearRTV(ColorOutput);
	record_cl->ClearDSVDepth(DepthStencilOutput);
	record_cl->SetViewport(&viewport);
	record_cl->SetScissor(&scissor);

	record_cl->BindCameraCBV(0, Scene->Camera);
	record_cl->BindTexture(2, texture);

	for (TW3DObject* object : Scene->objects) {
		record_cl->DrawObject(object, 1);
	}

	record_cl->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	record_cl->Close();
}

void TW3D::TW3DDefaultRenderer::Update() {
	for (TW3DObject* object : Scene->objects) {
		object->Update();
	}
}

void TW3D::TW3DDefaultRenderer::Execute(TWT::UInt BackBufferIndex) {
	TW3DGraphicsCommandList* command_list = current_record_index == 0 ? command_lists[BackBufferIndex * 2 + 1] : command_lists[BackBufferIndex * 2];

	while (command_list->IsEmpty())
		Sleep(1);

	ResourceManager->ExecuteCommandList(command_list);
}
