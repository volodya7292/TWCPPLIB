#pragma once
#include "TW3DResourceManager.h"

class TW3DVRSCalculator {
public:
	TW3DVRSCalculator(TW3DResourceManager* ResourceManager);
	~TW3DVRSCalculator();

	void Record(TW3DGraphicsCommandList* CommandList, TW3DRenderTarget* GBufferSpecular, TW3DRenderTarget* Output);

private:
	enum RootSignatureParams {

		NumParameters
	};
};


//class TW3DBitonicSorter {
//public:
//	TW3DBitonicSorter(TW3DResourceManager* ResourceManager);
//	~TW3DBitonicSorter();
//
//	void RecordSort(TW3DGraphicsCommandList* CommandList, TW3DBuffer* SortBuffer, TWT::uint ElementCount, bool SortAscending, bool IsPartiallyPreSorted);
//
//private:
//	enum RootSignatureParams {
//		GenericConstants,
//		ShaderSpecificConstants,
//		OutputUAV,
//		IndexBufferUAV,
//		NumParameters
//	};
//
//	TW3DRootSignature* m_pRootSignature;
//	TW3DResource* m_pDispatchArgs;
//	ID3D12CommandSignature* m_pCommandSignature;
//	const UINT cIndirectArgStride = 12;
//
//	TW3DComputePipelineState* m_pBitonicIndirectArgsCS;
//	TW3DComputePipelineState* m_pBitonicPreSortCS;
//	TW3DComputePipelineState* m_pBitonicInnerSortCS;
//	TW3DComputePipelineState* m_pBitonicOuterSortCS;
//};