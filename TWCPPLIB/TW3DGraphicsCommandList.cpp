#include "pch.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"

TW3D::TW3DGraphicsCommandList::TW3DGraphicsCommandList(TW3D::TW3DDevice* device, D3D12_COMMAND_LIST_TYPE type) {
	device->CreateCommandAllocator(type, &CommandAllocator);
	device->CreateGraphicsCommandList(type, CommandAllocator, &commandList);
}

TW3D::TW3DGraphicsCommandList::~TW3DGraphicsCommandList() {
	TWU::DXSafeRelease(commandList);
	TWU::DXSafeRelease(CommandAllocator);
}

ID3D12GraphicsCommandList* TW3D::TW3DGraphicsCommandList::Get() {
	return commandList;
}

void TW3D::TW3DGraphicsCommandList::UpdateSubresources(TW3DResource* DestinationResource, TW3DResource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource) {
	TWU::UpdateSubresourcesImp(commandList, DestinationResource->Get(), Intermediate->Get(), SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3D::TW3DGraphicsCommandList::UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource) {
	TWU::UpdateSubresourcesImp(commandList, DestinationResource, Intermediate, SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3D::TW3DGraphicsCommandList::ResourceBarrier(const D3D12_RESOURCE_BARRIER barrier) {
	commandList->ResourceBarrier(1, &barrier);
}

void TW3D::TW3DGraphicsCommandList::ResourceBarriers(const TWT::Vector<D3D12_RESOURCE_BARRIER>& barriers) {
	commandList->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void TW3D::TW3DGraphicsCommandList::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->Get(), StateBefore, StateAfter));
}

void TW3D::TW3DGraphicsCommandList::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, StateBefore, StateAfter));
}

void TW3D::TW3DGraphicsCommandList::SetPipelineState(TW3D::TW3DPipelineState* PipelineState) {
	commandList->SetPipelineState(PipelineState->Get());
	commandList->SetGraphicsRootSignature(PipelineState->RootSignature->Get());
}

void TW3D::TW3DGraphicsCommandList::SetRenderTarget(TW3DResourceRTV* RTV, TW3DResourceDSV* DSV) {
	commandList->OMSetRenderTargets(1, &RTV->GetHandle(), false, &DSV->GetHandle());
}

void TW3D::TW3DGraphicsCommandList::SetRenderTargets(TWT::UInt RTVDescriptorCount, const D3D12_CPU_DESCRIPTOR_HANDLE* RTVDescriptors, TW3DResourceDSV* DSV) {
	commandList->OMSetRenderTargets(RTVDescriptorCount, RTVDescriptors, false, &DSV->GetHandle());
}

void TW3D::TW3DGraphicsCommandList::ClearRTV(TW3DResourceRTV* RTV) {
	TWT::Vector4f clear = RTV->GetClearColor();
	float clearV[] = { clear.x, clear.y, clear.z, clear.w };
	commandList->ClearRenderTargetView(RTV->GetHandle(), clearV, 0, nullptr);
}

void TW3D::TW3DGraphicsCommandList::ClearDSVDepth(TW3DResourceDSV* DSV, TWT::Float Depth) {
	commandList->ClearDepthStencilView(DSV->GetHandle(), D3D12_CLEAR_FLAG_DEPTH, Depth, 0, 0, nullptr);
}

void TW3D::TW3DGraphicsCommandList::SetGraphicsRootSignature(TW3DRootSignature* RootSignature) {
	commandList->SetGraphicsRootSignature(RootSignature->Get());
}

void TW3D::TW3DGraphicsCommandList::SetDescriptorHeap(TW3DDescriptorHeap* heap) {
	ID3D12DescriptorHeap* nativeHeap = heap->Get();
	commandList->SetDescriptorHeaps(1, &nativeHeap);
}

void TW3D::TW3DGraphicsCommandList::SetDescriptorHeaps(TWT::Vector<TW3DDescriptorHeap*> heaps) {
	TWT::Vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());

	for (TWT::UInt i = 0; i < nativeHeaps.size(); i++)
		nativeHeaps[i] = heaps[i]->Get();

	commandList->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
}

void TW3D::TW3DGraphicsCommandList::SetGraphicsRootDescriptorTable(TWT::UInt RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {
	commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}

void TW3D::TW3DGraphicsCommandList::SetGraphicsRootCBV(TWT::UInt RootParameterIndex, TW3DResourceCB* CB, TWT::UInt ElementIndex) {
	commandList->SetGraphicsRootConstantBufferView(RootParameterIndex, CB->GetAddress(ElementIndex));
}

void TW3D::TW3DGraphicsCommandList::SetViewport(const D3D12_VIEWPORT* viewport) {
	commandList->RSSetViewports(1, viewport);
}

void TW3D::TW3DGraphicsCommandList::SetScissor(const D3D12_RECT* scissor) {
	commandList->RSSetScissorRects(1, scissor);
}

void TW3D::TW3DGraphicsCommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) {
	commandList->IASetPrimitiveTopology(PrimitiveTopology);
}

void TW3D::TW3DGraphicsCommandList::SetVertexBuffer(TWT::UInt StartSlot, TW3DResourceVB* view) {
	commandList->IASetVertexBuffers(StartSlot, 1, &view->GetView());
}

void TW3D::TW3DGraphicsCommandList::SetVertexBuffers(TWT::UInt StartSlot, TWT::Vector<D3D12_VERTEX_BUFFER_VIEW> views) {
	commandList->IASetVertexBuffers(StartSlot, static_cast<UINT>(views.size()), views.data());
}

void TW3D::TW3DGraphicsCommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view) {
	commandList->IASetIndexBuffer(view);
}

void TW3D::TW3DGraphicsCommandList::Draw(TWT::UInt VertexCountPerInstance, TWT::UInt StartVertexLocation, TWT::UInt InstanceCount, TWT::UInt StartInstanceLocation) {
	commandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void TW3D::TW3DGraphicsCommandList::DrawIndexed(TWT::UInt IndexCountPerInstance, TWT::UInt StartIndexLocation, TWT::UInt InstanceCount, TWT::UInt StartInstanceLocation, TWT::Int BaseVertexLocation) {
	commandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void TW3D::TW3DGraphicsCommandList::Reset() {
	TWU::SuccessAssert(CommandAllocator->Reset());
	TWU::SuccessAssert(commandList->Reset(CommandAllocator, nullptr));
}

void TW3D::TW3DGraphicsCommandList::Close() {
	TWU::SuccessAssert(commandList->Close());
}

TW3D::TW3DGraphicsCommandList* TW3D::TW3DGraphicsCommandList::CreateDirect(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}
