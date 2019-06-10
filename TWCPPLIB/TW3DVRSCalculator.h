#pragma once
#include "TW3DResourceManager.h"

class TW3DVRSCalculator {
public:
	TW3DVRSCalculator(TW3DResourceManager* ResourceManager);
	~TW3DVRSCalculator();

	void Record(TW3DGraphicsCommandList* CommandList, TW3DRenderTarget* GPosition,
		TW3DRenderTarget* GDiffuse, TW3DRenderTarget* GSpecular, TW3DRenderTarget* GNormal, TW3DTexture* Output);

private:
	enum RootSignatureParams {
		PositionTexture,
		DiffuseTexture,
		SpecularTexture,
		NormalTexture,
		OutputTexture,
		InputConstants,
		NumParameters
	};

	TW3DComputePipelineState* cp;
	TW3DRootSignature* root_signature;
};