#pragma once
#include "TW3DResourceManager.h"

class TW3DRaytracer {
public:
	TW3DRaytracer(TW3DResourceManager* ResourceManager);
	~TW3DRaytracer();

	void TraceRays(TW3DGraphicsCommandList* CommandList, TW3DBuffer* SortBuffer, TWT::uint ElementCount, bool SortAscending, bool IsPartiallyPreSorted);

private:
	enum RootSignatureParamsRT {
		
		NumParameters
	};

	enum RootSignatureParamsSVGFTA {
		
		NumParameters
	};

	enum RootSignatureParamsSVGFEV {
		
		NumParameters
	};

	enum RootSignatureParamsSVGFWF {
		
		NumParameters
	};

	TW3DComputePipelineState* m_pB;
	TW3DComputePipelineState* m_pBitonicPreSortCS;
	TW3DComputePipelineState* m_pBitonicInnerSortCS;
	TW3DComputePipelineState* m_pBitonicOuterSortCS;
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