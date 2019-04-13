#include "pch.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DSwapChain.h"
#include "TW3DGraphicsPipelineState.h"

TW3D::TW3DDefaultRenderer::~TW3DDefaultRenderer() {
}

void TW3D::TW3DDefaultRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height) {
	TW3DRenderer::Initialize(ResourceManager, SwapChain, Width, Height);

	TW3DDevice* device = ResourceManager->GetDevice();


	TW3DRootSignature* rootSignature = new TW3DRootSignature(
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	rootSignature->SetParameterCBV(0, D3D12_SHADER_VISIBILITY_VERTEX, 0);
	rootSignature->SetParameterCBV(1, D3D12_SHADER_VISIBILITY_VERTEX, 1);
	rootSignature->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	rootSignature->SetParameterSRV(3, D3D12_SHADER_VISIBILITY_PIXEL, 1);
	rootSignature->SetParameterSRV(4, D3D12_SHADER_VISIBILITY_PIXEL, 2);
	rootSignature->AddSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	//rootSignature->AddSampler(1, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0);
	rootSignature->Create(device);

	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> inputLayout(2);
	inputLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rastDesc.CullMode = D3D12_CULL_MODE_NONE;
	rastDesc.FrontCounterClockwise = TRUE;
	rastDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rastDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rastDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rastDesc.DepthClipEnable = FALSE;
	rastDesc.MultisampleEnable = FALSE;
	rastDesc.AntialiasedLineEnable = FALSE;
	rastDesc.ForcedSampleCount = 0;
	rastDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthDesc.StencilEnable = FALSE;
	depthDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	depthDesc.FrontFace = defaultStencilOp;
	depthDesc.BackFace = defaultStencilOp;

	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;

	opaque_raster_ps = new TW3D::TW3DGraphicsPipelineState(
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		SwapChain->GetDescription().SampleDesc,
		rastDesc,
		depthDesc,
		blendDesc,
		rootSignature,
		1);
	opaque_raster_ps->SetRTVFormat(0, DXGI_FORMAT_R8G8B8A8_UNORM);
	opaque_raster_ps->SetVertexShader("GBuffer.v.cso");
	opaque_raster_ps->SetPixelShader("GBuffer.p.cso");
	opaque_raster_ps->SetInputLayout(inputLayout);
	opaque_raster_ps->Create(device);


	rastDesc.CullMode = D3D12_CULL_MODE_NONE;
	depthDesc.DepthEnable = FALSE;


	TW3DRootSignature* blitRootSignature = new TW3D::TW3DRootSignature(
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);
	blitRootSignature->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	//blitRootSignature->SetParameter(0, TW3D::TW3DRootParameter(D3D12_SHADER_VISIBILITY_PIXEL, TWU::DXDescriptorRange(0, D3D12_DESCRIPTOR_RANGE_TYPE_SRV)));
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
	blit_ps->Create(device);



	//offscreen = new TW3D::TW3DResourceRTV(device, rtvDescriptorHeap, TW3D::TW3DSwapChain::BufferCount, mainDescriptorHeap, 2, DXGI_FORMAT_R8G8B8A8_UNORM, TWT::Vector4f(0, 0, 0, 1));
	//offscreen->Create(Width, Height);

	depthStencil = ResourceManager->CreateDepthStencilView(Width, Height);

	command_list = ResourceManager->CreateDirectCommandList();

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, Width, Height);
	scissor = CD3DX12_RECT(0, 0, Width, Height);
}

void TW3D::TW3DDefaultRenderer::Resize(TWT::UInt Width, TWT::UInt Height) {
	TW3DRenderer::Resize(Width, Height);

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, Width, Height);
	scissor = CD3DX12_RECT(0, 0, Width, Height);
}

void TW3D::TW3DDefaultRenderer::Record(TW3DScene* Scene, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStencilOutput) {
	command_list->Reset();

	command_list->SetPipelineState(opaque_raster_ps);
	command_list->BindResources(ResourceManager);

	command_list->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	command_list->SetRenderTarget(ColorOutput, DepthStencilOutput);
	command_list->ClearRTV(ColorOutput);
	command_list->ClearDSVDepth(DepthStencilOutput);
	command_list->SetViewport(&viewport);
	command_list->SetScissor(&scissor);
	command_list->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	command_list->BindCameraCBV(0, Scene->Camera);

	for (TW3DObject* object : Scene->objects) {
		command_list->DrawObject(object, 1);
	}

	command_list->ResourceBarrier(ColorOutput, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	command_list->Close();




	//commandList->Reset();

	//commandList->SetPipelineState(pipelineState);

	//commandList->BindResources(resource_manager);

	//commandList->SetRenderTargets({ offscreen, offscreen2 }, depthStencil);
	//const float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	//commandList->ClearRTV(offscreen);
	//commandList->ClearRTV(offscreen2);
	//commandList->ClearDSVDepth(depthStencil, 1.0f);

	//commandList->BindTexture(2, texture);
	//commandList->BindUAV(3, uavTex);
	//commandList->BindUAV(4, uavBuf);
	////commandList->BindTexture(3, texture2);

	//commandList->SetViewport(&viewport); // set the viewports
	//commandList->SetScissor(&scissorRect); // set the scissor rects
	//commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology

	//camera->Use(commandList);


	//commandList->DrawObject(cube, 1);


	//commandList->SetPipelineState(blitPipelineState);

	//commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//commandList->SetRenderTarget(renderTargets[frameIndex], depthStencil);
	//commandList->ClearRTV(renderTargets[frameIndex]);
	//commandList->ClearDSVDepth(depthStencil, 1.0f);
	//commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//commandList->BindRTVTexture(0, offscreen);

	//commandList->Draw(4);

	//commandList->ResourceBarrier(renderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//commandList->Close();
}

void TW3D::TW3DDefaultRenderer::Execute() {
	ResourceManager->ExecuteCommandList(command_list);
}
