#include "pch.h"
#include "TW3DMeshAABBCalculator.h"

TW3D::TW3DMeshAABBCalculator::TW3DMeshAABBCalculator(TW3DResourceManager* ResourceManager) {
	TW3DDevice* device = ResourceManager->GetDevice();

	TW3DRootSignature* root_signature = new TW3DRootSignature(false, false, false, false);
	root_signature->SetParameterUAVBuffer(InputBuffer, D3D12_SHADER_VISIBILITY_ALL, 2);
	root_signature->SetParameterConstants(InputConstants, D3D12_SHADER_VISIBILITY_ALL, 0, 1);
	root_signature->SetParameterUAVBuffer(InputAABBBuffer, D3D12_SHADER_VISIBILITY_ALL, 0);
	root_signature->SetParameterUAVBuffer(OutputBuffer, D3D12_SHADER_VISIBILITY_ALL, 1);


	/*m_pCalculateSceneAABBFromPrimitives = new TW3DComputePipelineState(root_signature);
	m_pCalculateSceneAABBFromPrimitives->SetShader(TW3DCompiledShader(BitonicSortIndirectArgs_ByteCode));
	m_pCalculateSceneAABBFromPrimitives->Create(device);

	m_pCalculateSceneAABBFromAABBs = new TW3DComputePipelineState(root_signature);
	m_pCalculateSceneAABBFromAABBs->SetShader(TW3DCompiledShader(BitonicSortIndirectArgs_ByteCode));
	m_pCalculateSceneAABBFromAABBs->Create(device);*/
}

TW3D::TW3DMeshAABBCalculator::~TW3DMeshAABBCalculator() {
}
