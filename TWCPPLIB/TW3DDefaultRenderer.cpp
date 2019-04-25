#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"
#include "TW3DGraphicsPipelineState.h"
#include "TW3DBitonicSorter.h"
TW3D::TW3DResourceSR* texture;

TW3D::TW3DDefaultRenderer::~TW3DDefaultRenderer() {
	delete gbuffer_ps;
	delete gvb_ps;
	/*delete morton_calc_ps;
	delete bb_calc_ps;
	delete setup_lbvh_nodes_ps;
	delete build_lbvh_splits_ps;
	delete update_lbvh_node_boundaries;*/
	delete rt_ps;
	delete blit_ps;

	delete lbvh_builder;
	//delete BitonicSorter;

	delete mesh_as_cl;

	delete rt_output;
	delete texture;
	delete gvb;
}

void TW3D::TW3DDefaultRenderer::CreateBlitResources() {
	TW3DRootSignature* root_signature = new TW3DRootSignature(false, false, true, false);
	root_signature->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_PIXEL, 0);  // Texture to blit
	root_signature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	root_signature->Create(Device);

	//TWT::Vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({ POSITION_ILE, TEXCOORD_ILE, NORMAL_ILE });

	D3D12_RASTERIZER_DESC rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_DEPTH_STENCIL_DESC depthDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	depthDesc.DepthEnable = false;

	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	blit_ps = new TW3D::TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		root_signature,
		1);
	blit_ps->SetRTVFormat(0, TWT::RGBA8Unorm);
	blit_ps->SetVertexShader("OffscreenBlit.v.cso");
	blit_ps->SetPixelShader("OffscreenBlit.p.cso");
	//blit_ps->SetInputLayout(input_layout);
	blit_ps->Create(Device);
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
	root_signature->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX, 0); // Global Vertex Buffer UAV
	root_signature->Create(Device);

	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> input_layout = CreateInputLayout({ POSITION_ILE, TEXCOORD_ILE, NORMAL_ILE });

	gvb_ps = new TW3D::TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, root_signature);
	gvb_ps->SetVertexShader("BuildGlobalVertexBuffer.v.cso");
	gvb_ps->SetInputLayout(input_layout);
	gvb_ps->Create(Device);

	gvb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(TWT::DefaultVertex));
}

void TW3D::TW3DDefaultRenderer::CreateRTResources() {
	rt_output = ResourceManager->CreateUnorderedAccessView(Width, Height, TWT::RGBA8Unorm);
	ResourceManager->ResourceBarrier(rt_output, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	TW3DRootSignature* root_signature2 = new TW3DRootSignature(false, false, false, false);
	root_signature2->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	root_signature2->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // LBVH nodes buffer SRV
	root_signature2->SetParameterUAVTexture(2, D3D12_SHADER_VISIBILITY_ALL, 0); // Output texture
	root_signature2->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	root_signature2->SetParameterCBV(4, D3D12_SHADER_VISIBILITY_ALL, 1); // Camera CBV
	root_signature2->Create(Device);

	rt_ps = new TW3DComputePipelineState(root_signature2);
	rt_ps->SetShader("RayTrace.c.cso");
	rt_ps->Create(Device);

	lbvh_builder = new TW3DLBVHBuilder(ResourceManager);
	mesh_as_cl = ResourceManager->CreateComputeCommandList();
}

void TW3D::TW3DDefaultRenderer::BlitOutput(TW3DGraphicsCommandList* cl, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* Depth) {
	cl->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	cl->ResourceBarrier(rt_output, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	cl->SetPipelineState(blit_ps);
	cl->SetRenderTarget(ColorOutput, Depth);
	cl->BindUAVTextureSRV(0, rt_output);
	cl->ClearRTV(ColorOutput);
	cl->ClearDSVDepth(Depth);
	cl->SetViewport(&viewport);
	cl->SetScissor(&scissor);
	cl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cl->Draw(4);

	cl->ResourceBarrier(rt_output, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	cl->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void TW3D::TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Width, Height);
	CreateBlitResources();
	CreateGBufferResources();
	CreateGVBResources();
	CreateRTResources();

	texture = ResourceManager->CreateTextureArray2D(720, 720, 10, DXGI_FORMAT_R8G8B8A8_UNORM);
	texture->Upload2D(L"D:/����.png", 0);
	texture->Upload2D(L"D:/����2.png", 1);
	//texture = ResourceManager->CreateTexture2D(L"D:/����.png");
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


	// Build Global Vertex Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	record_cl->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	record_cl->SetPipelineState(gvb_ps);
	record_cl->BindUAVBuffer(1, gvb);
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
	record_cl->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);


	// Build Vertex Mesh acceleration structures
	// -------------------------------------------------------------------------------------------------------------------------

	// Render GBuffer
	// -------------------------------------------------------------------------------------------------------------------------
	/*record_cl->SetPipelineState(gbuffer_ps);

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

	record_cl->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);*/


	BlitOutput(record_cl, ColorOutput, DepthStencilOutput);

	record_cl->Close();
}

void TW3D::TW3DDefaultRenderer::Update() {
	for (TW3DObject* object : Scene->objects) {
		object->Update();
	}
}

bool build = false;
void TW3D::TW3DDefaultRenderer::Execute(TWT::UInt BackBufferIndex) {
	TW3DRenderer::Execute(BackBufferIndex);
	ResourceManager->ExecuteCommandList(execute_cl);
	ResourceManager->FlushCommandLists();

	TW3DVertexMesh* mesh = nullptr;

	gvb_vertex_meshes.clear();
	for (TW3DObject* object : Scene->objects)
		if (gvb_vertex_meshes.find(object->VMInstance.VertexMesh) == gvb_vertex_meshes.end()) {
			gvb_vertex_meshes.emplace(object->VMInstance.VertexMesh);
			mesh = object->VMInstance.VertexMesh;
			//BuildVMAccelerationStructure(object->VMInstance.VertexMesh);
		}



	ResourceManager->FlushCommandList(mesh_as_cl);
	mesh_as_cl->Reset();
	mesh_as_cl->BindResources(ResourceManager);

	if (!build) {
		lbvh_builder->Build(mesh_as_cl, gvb, mesh);
		build = true;
	}

	// Trace rays
	mesh_as_cl->SetPipelineState(rt_ps);
	mesh_as_cl->BindUAVBufferSRV(0, gvb);
	mesh_as_cl->BindUAVBufferSRV(1, mesh->GetLBVHNodeBufferResource());
	mesh_as_cl->BindUAVTexture(2, rt_output);
	mesh_as_cl->SetRoot32BitConstant(3, mesh->GetGVBVertexOffset(), 0);
	mesh_as_cl->SetRoot32BitConstant(3, mesh->GetTriangleCount(), 1);
	mesh_as_cl->SetRootCBV(4, Scene->Camera->GetConstantBuffer());
	mesh_as_cl->Dispatch(ceil(Width / 8.0f), ceil(Height / 8.0f));

	mesh_as_cl->Close();
	ResourceManager->ExecuteCommandList(mesh_as_cl);
}
