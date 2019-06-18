#include "pch.h"
#include "TW3DBlitter.h"
#include "CompiledShaders/ScreenQuad.v.h"
#include "CompiledShaders/Blit.p.h"

TW3DBlitter::TW3DBlitter(TW3DResourceManager* ResourceManager) {
	auto device = ResourceManager->GetDevice();

	TW3DRootSignature* root_signature = new TW3DRootSignature(device,
		{
			TW3DRPTexture(0, D3D12_SHADER_VISIBILITY_PIXEL, 0), // Texture to blit
		},
		true, true, false, false
		);

	ps = new TW3DGraphicsPipelineState(root_signature);
	ps->SetRTVFormat(0, TWT::RGBA32Float);
	ps->SetVertexShader(new TW3DShader(TW3DCompiledShader(ScreenQuad_VertexByteCode), "ScreenQuad"s));
	ps->SetPixelShader(new TW3DShader(TW3DCompiledShader(Blit_PixelByteCode), "BlitPixel"s));
	ps->Create(device);
}

TW3DBlitter::~TW3DBlitter() {
	delete ps;
}

void TW3DBlitter::Blit(TW3DGraphicsCommandList* CL, TW3DRenderTarget* RenderTarget, TW3DTexture* Texture) {
	CL->SetPipelineState(ps);
	CL->SetViewportScissor(RenderTarget->GetSize());
	CL->BindTexture(0, Texture);
	CL->SetRenderTarget(RenderTarget);
	CL->DrawQuad();
}

void TW3DBlitter::Blit(TW3DGraphicsCommandList* CL, TW3DRenderTarget* SrcRenderTarget, TW3DRenderTarget* DstRenderTarget) {
	CL->ResourceBarrier(SrcRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE);
	CL->ResourceBarrier(DstRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
	CL->CopyTextureRegion(DstRenderTarget, SrcRenderTarget);
	CL->ResourceBarrier(SrcRenderTarget, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CL->ResourceBarrier(DstRenderTarget, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
	/*CL->SetPipelineState(ps);
	CL->SetViewportScissor(SrcRenderTarget->GetSize());
	CL->BindTexture(0, SrcRenderTarget);
	CL->SetRenderTarget(DstRenderTarget);
	CL->DrawQuad();*/
	//CL->ResourceBarrier(SrcRenderTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}
