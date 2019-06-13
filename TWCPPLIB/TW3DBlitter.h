#pragma once
#include "TW3DResourceManager.h"

class TW3DBlitter {
public:
	TW3DBlitter(TW3DResourceManager* ResourceManager);
	~TW3DBlitter();

	// CL : Graphics command list
	void Blit(TW3DGraphicsCommandList* CL, TW3DRenderTarget* RenderTarget, TW3DTexture* Texture);
	// CL : Graphics command list
	void Blit(TW3DGraphicsCommandList* CL, TW3DRenderTarget* SrcRenderTarget, TW3DRenderTarget* DstRenderTarget);

private:
	TW3DGraphicsPipelineState* ps;
};