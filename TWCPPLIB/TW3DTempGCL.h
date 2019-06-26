#pragma once
#include "TW3DCommandQueue.h"

class TW3DTempGCL {
public:
	TW3DTempGCL(TW3DDevice* Device);
	~TW3DTempGCL();

	TW3DCommandList* Get();

	void UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
		TWT::uint SubresourcesCount = 1, TWT::uint64 IntermediateOffset = 0, TWT::uint FirstSubresource = 0);
	void CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount);
	void ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	void Reset();
	void Execute();

private:
	TW3DCommandQueue* CommandQueue;
	TW3DCommandList* CommandList;
};