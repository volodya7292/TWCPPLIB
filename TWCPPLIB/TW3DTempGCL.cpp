#include "pch.h"
#include "TW3DTempGCL.h"
#include "TW3DResource.h"

TW3DTempGCL::TW3DTempGCL(TW3DDevice* Device) {
	CommandQueue = new TW3DCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	CommandList = new TW3DCommandList(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3DTempGCL::~TW3DTempGCL() {
	delete CommandList;
	delete CommandQueue;
}

TW3DCommandList* TW3DTempGCL::Get() {
	return CommandList;
}

void TW3DTempGCL::UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
	TWT::uint SubresourcesCount, TWT::uint64 intermediateOffset, TWT::uint FirstSubresource) {
	CommandList->UpdateSubresources(DestinationResource, intermediate, SrcData, SubresourcesCount, intermediateOffset, FirstSubresource);
}

void TW3DTempGCL::CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount) {
	CommandList->CopyBufferRegion(DstBuffer, DstOffset, SrcBuffer, SrcOffset, ByteCount);
}

void TW3DTempGCL::CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION const* Dst, TWT::uint3 DstXYZ, D3D12_TEXTURE_COPY_LOCATION const* Src, D3D12_BOX const* SrcBox) {
	CommandList->CopyTextureRegion(Dst, DstXYZ, Src, SrcBox);
}

void TW3DTempGCL::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	CommandList->ResourceBarrier(Resource, StateBefore, StateAfter);
}

void TW3DTempGCL::Reset() {
	CommandList->Reset();
}

void TW3DTempGCL::Execute() {
	CommandList->Close();
	CommandQueue->ExecuteCommandList(CommandList);
	CommandQueue->FlushCommandList(CommandList);
}
