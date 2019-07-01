#pragma once
#include "TW3DResourceManager.h"

class TW3DBitonicSorter {
public:
	TW3DBitonicSorter(TW3DResourceManager* ResourceManager);
	~TW3DBitonicSorter();

	void RecordSort(TW3DCommandList* CommandList, TW3DBuffer* SortBuffer, TWT::uint ElementCount, bool SortAscending, bool IsPartiallyPreSorted);

private:
	enum RootSignatureParams {
		GenericConstants,
		ShaderSpecificConstants,
		OutputUAV,
		//IndexBufferUAV,
		NumParameters
	};

	TW3DRootSignature* m_pRootSignature;
	TW3DResource* m_pDispatchArgs;
	ID3D12CommandSignature* m_pCommandSignature;
	const UINT cIndirectArgStride = 12;

	TW3DComputePipelineState* m_pBitonicIndirectArgsCS;
	TW3DComputePipelineState* m_pBitonicPreSortCS;
	TW3DComputePipelineState* m_pBitonicInnerSortCS;
	TW3DComputePipelineState* m_pBitonicOuterSortCS;
};