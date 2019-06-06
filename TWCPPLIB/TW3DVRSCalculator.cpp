#include "pch.h"
#include "TW3DVRSCalculator.h"
#include "CompiledShaders/CalculateVariableRateShading.c.h"

TW3DVRSCalculator::TW3DVRSCalculator(TW3DResourceManager* ResourceManager) {
	TW3DDevice* device = ResourceManager->GetDevice();

	root_signature = new TW3DRootSignature(device,
		{
			TW3DRPTexture(PositionTexture, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPTexture(DiffuseTexture, D3D12_SHADER_VISIBILITY_ALL, 1),
			TW3DRPTexture(SpecularTexture, D3D12_SHADER_VISIBILITY_ALL, 2),
			TW3DRPTexture(NormalTexture, D3D12_SHADER_VISIBILITY_ALL, 3),
			TW3DRPTexture(OutputTexture, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(InputConstants, D3D12_SHADER_VISIBILITY_ALL, 0, 1),
		},
		{ TW3DStaticSampler(D3D12_SHADER_VISIBILITY_ALL, 0, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0) },
		false, false, true, false
		);

	cp = new TW3DComputePipelineState(root_signature);
	cp->SetShader(new TW3DShader(TW3DCompiledShader(CalculateVariableRateShading_ByteCode), "CalculateVariableRateShading"s));
	cp->Create(device);
}

TW3DVRSCalculator::~TW3DVRSCalculator() {
	delete cp;
}

void TW3DVRSCalculator::Record(TW3DGraphicsCommandList* CommandList, TW3DRenderTarget* GPosition,
	TW3DRenderTarget* GDiffuse,TW3DRenderTarget* GSpecular, TW3DRenderTarget* GNormal, TW3DTexture* Output) {

	CommandList->SetPipelineState(cp);

	CommandList->BindTexture(PositionTexture, GPosition);
	CommandList->BindTexture(DiffuseTexture, GDiffuse);
	CommandList->BindTexture(SpecularTexture, GSpecular);
	CommandList->BindTexture(NormalTexture, GNormal);
	CommandList->BindTexture(OutputTexture, Output, true);

	TWT::vec2u size = Output->GetSize();

	for (TWT::uint i = 0; i < 4; i++) {
		float kernel = 1 << (i + 1);
		TWT::vec2u gn = TWT::vec2u(ceil(size.x / kernel / 8.0f), ceil(size.y / kernel / 8.0f));

		CommandList->Bind32BitConstant(InputConstants, i, 0);
		CommandList->Dispatch(gn.x, gn.y);
		CommandList->ResourceBarrier(TW3DUAVBarrier());
	}
}
