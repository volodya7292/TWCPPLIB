#include "pch.h"
#include "TW3DTempGCL.h"

TW3D::TW3DTempGCL::TW3DTempGCL(TW3DDevice* Device) {
	Fence = new TW3DFence(Device);
	CommandQueue = new TW3DCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	CommandList = new TW3DGraphicsCommandList(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	CommandList->Close();
}

TW3D::TW3DTempGCL::~TW3DTempGCL() {
	delete CommandList;
	delete CommandQueue;
	delete Fence;
}

void TW3D::TW3DTempGCL::UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
	TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource) {
	CommandList->UpdateSubresources(DestinationResource, Intermediate, SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3D::TW3DTempGCL::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	CommandList->ResourceBarrier(Resource, StateBefore, StateAfter);
}

void TW3D::TW3DTempGCL::Reset() {
	CommandList->Reset();
}

void TW3D::TW3DTempGCL::Execute() {
	CommandList->Close();
	CommandQueue->ExecuteCommandList(CommandList);
	Fence->Flush(CommandQueue);
}
