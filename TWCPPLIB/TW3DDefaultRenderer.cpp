#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"
#include "TW3DGraphicsPipelineState.h"
#include "TW3DBitonicSorter.h"
//TW3D::TW3DResourceSR* texture;

TW3D::TW3DDefaultRenderer::~TW3DDefaultRenderer() {
	delete gbuffer_ps;
	delete rt_ps;
	delete blit_ps;

	delete rt_cl;

	delete rt_output;
	//delete texture;
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

void TW3D::TW3DDefaultRenderer::CreateRTResources() {
	rt_output = ResourceManager->CreateUnorderedAccessView(Width, Height, TWT::RGBA8Unorm);
	ResourceManager->ResourceBarrier(rt_output, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	TW3DRootSignature* rs = new TW3DRootSignature(false, false, false, false);
	rs->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Scene LBVH buffer SRV
	rs->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Global Node Buffer SRV
	rs->SetParameterUAVTexture(3, D3D12_SHADER_VISIBILITY_ALL, 0); // Output texture
	rs->SetParameterCBV(4, D3D12_SHADER_VISIBILITY_ALL, 0); // Camera CBV
	rs->Create(Device);

	rt_ps = new TW3DComputePipelineState(rs);
	rt_ps->SetShader("RayTrace.c.cso");
	rt_ps->Create(Device);

	rt_cl = ResourceManager->CreateComputeCommandList();
}

void TW3D::TW3DDefaultRenderer::BlitOutput(TW3DGraphicsCommandList* cl, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* Depth) {
	cl->ResourceBarriers({
		TW3DTransitionBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET),
		TW3DTransitionBarrier(rt_output, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	});

	cl->SetPipelineState(blit_ps);
	cl->SetRenderTarget(ColorOutput, Depth);
	cl->BindUAVSRV(0, rt_output);
	cl->ClearRTV(ColorOutput, TWT::Vector4f(0, 0, 0, 1));
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

void TW3D::TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Width, Height);
	TWU::TW3DLogInfo("TW3DRenderer initialized."s);
	CreateBlitResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] BlitResources initialized."s);
	CreateGBufferResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] GBufferResources initialized."s);
	CreateRTResources();
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] RTResources initialized."s);

	//texture = ResourceManager->CreateTextureArray2D(720, 720, 10, DXGI_FORMAT_R8G8B8A8_UNORM);
	TWU::TW3DLogInfo("[TW3DDefaultRenderer] 'texture' initialized."s);

	//texture->Upload2D(L"D:/тест.png", 0);
	//texture->Upload2D(L"D:/тест2.png", 1);
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

void TW3D::TW3DDefaultRenderer::RecordBeforeExecution() {
	Scene->RecordBeforeExecution();

	// Trace rays
	// -------------------------------------------------------------------------------------------------------------------------
	ResourceManager->FlushCommandList(rt_cl);
	rt_cl->Reset();
	rt_cl->BindResources(ResourceManager);
	rt_cl->SetPipelineState(rt_ps);
	Scene->Bind(rt_cl, 0, 1, 2);
	rt_cl->BindUAVTexture(3, rt_output);
	rt_cl->SetRootCBV(4, Scene->Camera->GetConstantBuffer());
	rt_cl->Dispatch(ceil(Width / 8.0f), ceil(Height / 8.0f));
	rt_cl->ResourceBarrier(TW3DUAVBarrier());
	rt_cl->Close();
}

void TW3D::TW3DDefaultRenderer::Update() {
	for (TW3DObject* object : Scene->Objects) {
		object->Update();
	}
}

void TW3D::TW3DDefaultRenderer::Execute(TWT::UInt BackBufferIndex) {
	TW3DRenderer::Execute(BackBufferIndex);

	ResourceManager->ExecuteCommandList(rt_cl);
	ResourceManager->FlushCommandLists();
	ResourceManager->ExecuteCommandList(execute_cl);
	//ResourceManager->FlushCommandLists();
}
