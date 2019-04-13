#include "pch.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DResourceSR.h"
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"
#include "TW3DResourceManager.h"
#include "TW3DObject.h"

TW3D::TW3DGraphicsCommandList::TW3DGraphicsCommandList(TW3D::TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) :
	type(Type)
{
	Device->CreateCommandAllocator(type, &command_allocator);
	Device->CreateGraphicsCommandList(type, command_allocator, &command_list);
}

TW3D::TW3DGraphicsCommandList::~TW3DGraphicsCommandList() {
	TWU::DXSafeRelease(command_list);
	TWU::DXSafeRelease(command_allocator);
}

ID3D12GraphicsCommandList* TW3D::TW3DGraphicsCommandList::Get() {
	return command_list;
}

void TW3D::TW3DGraphicsCommandList::UpdateSubresources(TW3DResource* DestinationResource, TW3DResource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource) {
	TWU::UpdateSubresourcesImp(command_list, DestinationResource->Get(), Intermediate->Get(), SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3D::TW3DGraphicsCommandList::UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::UInt SubresourcesCount, TWT::UInt64 IntermediateOffset, TWT::UInt FirstSubresource) {
	TWU::UpdateSubresourcesImp(command_list, DestinationResource, Intermediate, SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3D::TW3DGraphicsCommandList::ResourceBarrier(const D3D12_RESOURCE_BARRIER barrier) {
	command_list->ResourceBarrier(1, &barrier);
}

void TW3D::TW3DGraphicsCommandList::ResourceBarriers(const TWT::Vector<D3D12_RESOURCE_BARRIER>& barriers) {
	command_list->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void TW3D::TW3DGraphicsCommandList::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->Get(), StateBefore, StateAfter));
}

void TW3D::TW3DGraphicsCommandList::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, StateBefore, StateAfter));
}

void TW3D::TW3DGraphicsCommandList::SetPipelineState(TW3D::TW3DGraphicsPipelineState* PipelineState) {
	command_list->SetPipelineState(PipelineState->Get());
	command_list->SetGraphicsRootSignature(PipelineState->RootSignature->Get());
}

void TW3D::TW3DGraphicsCommandList::SetPipelineState(TW3D::TW3DComputePipelineState* PipelineState) {
	command_list->SetPipelineState(PipelineState->Get());
	command_list->SetComputeRootSignature(PipelineState->RootSignature->Get());
}

void TW3D::TW3DGraphicsCommandList::SetRenderTarget(TW3DResourceRTV* RTV, TW3DResourceDSV* DSV) {
	command_list->OMSetRenderTargets(1, &RTV->GetRTVCPUHandle(), false, &DSV->GetCPUHandle());
}

void TW3D::TW3DGraphicsCommandList::SetRenderTargets(const TWT::Vector<TW3DResourceRTV*>& RTVs, TW3DResourceDSV* DSV) {
	TWT::Vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(RTVs.size());
	for (int i = 0; i < RTVs.size(); i++)
		handles[i] = RTVs[i]->GetRTVCPUHandle();
	command_list->OMSetRenderTargets(static_cast<UINT>(RTVs.size()), handles.data(), false, &DSV->GetCPUHandle());
}

void TW3D::TW3DGraphicsCommandList::ClearRTV(TW3DResourceRTV* RTV) {
	TWT::Vector4f clear = RTV->GetClearColor();
	float clearV[] = { clear.x, clear.y, clear.z, clear.w };
	command_list->ClearRenderTargetView(RTV->GetRTVCPUHandle(), clearV, 0, nullptr);
}

void TW3D::TW3DGraphicsCommandList::ClearDSVDepth(TW3DResourceDSV* DSV, TWT::Float Depth) {
	command_list->ClearDepthStencilView(DSV->GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, Depth, 0, 0, nullptr);
}

void TW3D::TW3DGraphicsCommandList::SetGraphicsRootSignature(TW3DRootSignature* RootSignature) {
	command_list->SetGraphicsRootSignature(RootSignature->Get());
}

void TW3D::TW3DGraphicsCommandList::SetDescriptorHeap(TW3DDescriptorHeap* heap) {
	ID3D12DescriptorHeap* nativeHeap = heap->Get();
	command_list->SetDescriptorHeaps(1, &nativeHeap);
}

void TW3D::TW3DGraphicsCommandList::SetDescriptorHeaps(TWT::Vector<TW3DDescriptorHeap*> heaps) {
	TWT::Vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());

	for (TWT::UInt i = 0; i < nativeHeaps.size(); i++)
		nativeHeaps[i] = heaps[i]->Get();

	command_list->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
}

void TW3D::TW3DGraphicsCommandList::SetGraphicsRootDescriptorTable(TWT::UInt RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {
	command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}

void TW3D::TW3DGraphicsCommandList::SetGraphicsRootCBV(TWT::UInt RootParameterIndex, TW3DResourceCB* CB, TWT::UInt ElementIndex) {
	command_list->SetGraphicsRootConstantBufferView(RootParameterIndex, CB->GetAddress(ElementIndex));
}

void TW3D::TW3DGraphicsCommandList::SetViewport(const D3D12_VIEWPORT* viewport) {
	command_list->RSSetViewports(1, viewport);
}

void TW3D::TW3DGraphicsCommandList::SetScissor(const D3D12_RECT* scissor) {
	command_list->RSSetScissorRects(1, scissor);
}

void TW3D::TW3DGraphicsCommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) {
	command_list->IASetPrimitiveTopology(PrimitiveTopology);
}

void TW3D::TW3DGraphicsCommandList::SetVertexBuffer(TWT::UInt StartSlot, TW3DResourceVB* view) {
	command_list->IASetVertexBuffers(StartSlot, 1, &view->GetView());
}

void TW3D::TW3DGraphicsCommandList::SetVertexBuffers(TWT::UInt StartSlot, TWT::Vector<D3D12_VERTEX_BUFFER_VIEW> views) {
	command_list->IASetVertexBuffers(StartSlot, static_cast<UINT>(views.size()), views.data());
}

void TW3D::TW3DGraphicsCommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view) {
	command_list->IASetIndexBuffer(view);
}

void TW3D::TW3DGraphicsCommandList::Draw(TWT::UInt VertexCountPerInstance, TWT::UInt StartVertexLocation, TWT::UInt InstanceCount, TWT::UInt StartInstanceLocation) {
	command_list->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void TW3D::TW3DGraphicsCommandList::DrawIndexed(TWT::UInt IndexCountPerInstance, TWT::UInt StartIndexLocation, TWT::UInt InstanceCount, TWT::UInt StartInstanceLocation, TWT::Int BaseVertexLocation) {
	command_list->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void TW3D::TW3DGraphicsCommandList::Dispatch(TWT::UInt ThreadGroupCountX, TWT::UInt ThreadGroupCountY, TWT::UInt ThreadGroupCountZ) {
	command_list->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void TW3D::TW3DGraphicsCommandList::BindResources(TW3DResourceManager* ResourceManager) {
	SetDescriptorHeap(ResourceManager->GetSVDescriptorHeap());
}

void TW3D::TW3DGraphicsCommandList::BindTexture(TWT::UInt RootParameterIndex, TW3DResourceSR* SR) {
	command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, SR->GetGPUHandle());
}

void TW3D::TW3DGraphicsCommandList::BindRTVTexture(TWT::UInt RootParameterIndex, TW3DResourceRTV* RTV) {
	command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, RTV->GetSRVGPUHandle());
}

void TW3D::TW3DGraphicsCommandList::BindUAV(TWT::UInt RootParameterIndex, TW3DResourceUAV* UAV) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootDescriptorTable(RootParameterIndex, UAV->GetGPUHandle());
	else
		command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, UAV->GetGPUHandle());
}

void TW3D::TW3DGraphicsCommandList::DrawObject(TW3DObject* object, TWT::UInt ModelCBRootParameterIndex) {
	object->RecordDraw(this, ModelCBRootParameterIndex);
}

void TW3D::TW3DGraphicsCommandList::Reset() {
	TWU::SuccessAssert(command_allocator->Reset());
	TWU::SuccessAssert(command_list->Reset(command_allocator, nullptr));
}

void TW3D::TW3DGraphicsCommandList::Close() {
	TWU::SuccessAssert(command_list->Close());
}

TW3D::TW3DGraphicsCommandList* TW3D::TW3DGraphicsCommandList::CreateDirect(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3D::TW3DGraphicsCommandList* TW3D::TW3DGraphicsCommandList::CreateCompute(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}
