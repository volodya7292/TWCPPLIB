#pragma once
#include "TW3DCommandQueue.h"

class TW3DTempGCL {
public:
	TW3DTempGCL(TW3DDevice* Device);
	~TW3DTempGCL();

	TW3DCommandList* Get();

	void CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount);
	void CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION const* Dst, TWT::uint3 DestXYZ, D3D12_TEXTURE_COPY_LOCATION const* Src, D3D12_BOX const* SrcBox = nullptr);
	void ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	void Reset();
	void Execute();

private:
	TW3DCommandQueue* CommandQueue;
	TW3DCommandList* CommandList;
};