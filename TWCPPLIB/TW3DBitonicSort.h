#pragma once
#include "TW3DResourceManager.h"

namespace TW3D {
	class TW3DBitonicSort {
	public:
		TW3DBitonicSort(TW3DResourceManager* ResourceManager);
		~TW3DBitonicSort();

		void RecordSort(TW3DGraphicsCommandList* CommandList, TW3DResourceUAV* SortKeyBuffer, TW3DResourceUAV* IndexBuffer,
			TWT::UInt ElementCount, bool SortAscending);

	private:
		enum RootSignatureParams {
			GenericConstants,
			ShaderSpecificConstants,
			OutputUAV,
			IndexBufferUAV,
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
}