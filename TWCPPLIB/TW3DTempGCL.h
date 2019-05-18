#pragma once
#include "TW3DCommandQueue.h"

class TW3DTempGCL {
public:
	TW3DTempGCL(TW3DDevice* Device);
	~TW3DTempGCL();

	TW3DGraphicsCommandList* Get();

	void UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
		TWT::UInt SubresourcesCount = 1, TWT::UInt64 IntermediateOffset = 0, TWT::UInt FirstSubresource = 0);
	void ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	void Reset();
	void Execute();

private:
	TW3DCommandQueue* CommandQueue;
	TW3DGraphicsCommandList* CommandList;
};