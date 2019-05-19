#include "pch.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DTexture.h"
#include "TW3DVertexBuffer.h"
#include "TW3DConstantBuffer.h"
#include "TW3DRenderTarget.h"
#include "TW3DBuffer.h"
#include "TW3DResourceDSV.h"
#include "TW3DResourceManager.h"
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"

TW3DGraphicsCommandList::TW3DGraphicsCommandList(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) :
	type(Type)
{
	Device->CreateCommandAllocator(Type, &command_allocator);
	Device->CreateGraphicsCommandList(Type, command_allocator, &command_list);
	command_list->SetName(L"TW3DGraphicsCommandList");
	command_allocator->SetName(L"TW3D ID3D12CommandAllocator");
	command_list->Close();
}

TW3DGraphicsCommandList::~TW3DGraphicsCommandList() {
	TWU::DXSafeRelease(command_list);
	TWU::DXSafeRelease(command_allocator);
}

ID3D12GraphicsCommandList* TW3DGraphicsCommandList::Get() {
	return command_list;
}

D3D12_COMMAND_LIST_TYPE TW3DGraphicsCommandList::GetType() {
	return type;
}

void TW3DGraphicsCommandList::UpdateSubresources(TW3DResource* DestinationResource, TW3DResource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::uint SubresourcesCount, TWT::uint64 IntermediateOffset, TWT::uint FirstSubresource) {
	TWU::UpdateSubresourcesImp(command_list, DestinationResource->Get(), Intermediate->Get(), SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3DGraphicsCommandList::UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::uint SubresourcesCount, TWT::uint64 IntermediateOffset, TWT::uint FirstSubresource) {
	TWU::UpdateSubresourcesImp(command_list, DestinationResource, Intermediate, SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3DGraphicsCommandList::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
	command_list->ResourceBarrier(1, &barrier);
}

void TW3DGraphicsCommandList::ResourceBarriers(const std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	command_list->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void TW3DGraphicsCommandList::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->Get(), StateBefore, StateAfter));
}

void TW3DGraphicsCommandList::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, StateBefore, StateAfter));
}

void TW3DGraphicsCommandList::CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount) {
	command_list->CopyBufferRegion(DstBuffer->Get(), DstOffset, SrcBuffer->Get(), SrcOffset, ByteCount);
}

void TW3DGraphicsCommandList::SetPipelineState(TW3DGraphicsPipelineState* PipelineState) {
	command_list->SetPipelineState(PipelineState->Get());
	SetRootSignature(PipelineState->RootSignature);
}

void TW3DGraphicsCommandList::SetPipelineState(TW3DComputePipelineState* PipelineState) {
	command_list->SetPipelineState(PipelineState->Get());
	SetRootSignature(PipelineState->RootSignature);
}

void TW3DGraphicsCommandList::SetRenderTarget(TW3DRenderTarget* RenderTarget, TW3DResourceDSV* DSV) {
	command_list->OMSetRenderTargets(1, &RenderTarget->GetRTVCPUHandle(), false, &DSV->GetCPUHandle());
}

void TW3DGraphicsCommandList::SetRenderTargets(const std::vector<TW3DRenderTarget*>& RTVs, TW3DResourceDSV* DSV) {
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(RTVs.size());
	for (int i = 0; i < RTVs.size(); i++)
		handles[i] = RTVs[i]->GetRTVCPUHandle();
	command_list->OMSetRenderTargets(static_cast<UINT>(RTVs.size()), handles.data(), false, &DSV->GetCPUHandle());
}

void TW3DGraphicsCommandList::ClearRTV(TW3DRenderTarget* RenderTarget) {
	TWT::vec4 clear = RenderTarget->GetClearColor();
	float clearV[] = { clear.x, clear.y, clear.z, clear.w };
	command_list->ClearRenderTargetView(RenderTarget->GetRTVCPUHandle(), clearV, 0, nullptr);
}

void TW3DGraphicsCommandList::ClearRTV(TW3DRenderTarget* RenderTarget, TWT::vec4 Color) {
	float clearV[] = { Color.x, Color.y, Color.z, Color.w };
	command_list->ClearRenderTargetView(RenderTarget->GetRTVCPUHandle(), clearV, 0, nullptr);
}

void TW3DGraphicsCommandList::ClearDSVDepth(TW3DResourceDSV* DSV, float Depth) {
	command_list->ClearDepthStencilView(DSV->GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, Depth, 0, 0, nullptr);
}

void TW3DGraphicsCommandList::SetRootSignature(TW3DRootSignature* RootSignature) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootSignature(RootSignature->Get());
	else
		command_list->SetGraphicsRootSignature(RootSignature->Get());
}

void TW3DGraphicsCommandList::SetDescriptorHeap(TW3DDescriptorHeap* heap) {
	ID3D12DescriptorHeap* nativeHeap = heap->Get();
	command_list->SetDescriptorHeaps(1, &nativeHeap);
}

void TW3DGraphicsCommandList::SetDescriptorHeaps(std::vector<TW3DDescriptorHeap*> heaps) {
	std::vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());

	for (TWT::uint i = 0; i < nativeHeaps.size(); i++)
		nativeHeaps[i] = heaps[i]->Get();

	command_list->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
}

void TW3DGraphicsCommandList::SetRootDescriptorTable(TWT::uint RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootDescriptorTable(RootParameterIndex, BaseDescriptor);
	else
		command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}

void TW3DGraphicsCommandList::SetRoot32BitConstant(TWT::uint RootParameterIndex, TWT::uint Data, TWT::uint DestOffsetIn32BitValues) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRoot32BitConstant(RootParameterIndex, Data, DestOffsetIn32BitValues);
	else
		command_list->SetGraphicsRoot32BitConstant(RootParameterIndex, Data, DestOffsetIn32BitValues);
}

void TW3DGraphicsCommandList::SetRoot32BitConstants(TWT::uint RootParameterIndex, TWT::uint Num32BitValuesToSet, const void* Data, TWT::uint DestOffsetIn32BitValues) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, Data, DestOffsetIn32BitValues);
	else
		command_list->SetGraphicsRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, Data, DestOffsetIn32BitValues);
}

void TW3DGraphicsCommandList::SetViewport(const D3D12_VIEWPORT* viewport) {
	command_list->RSSetViewports(1, viewport);
}

void TW3DGraphicsCommandList::SetScissor(const D3D12_RECT* scissor) {
	command_list->RSSetScissorRects(1, scissor);
}

void TW3DGraphicsCommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) {
	command_list->IASetPrimitiveTopology(PrimitiveTopology);
}

void TW3DGraphicsCommandList::SetVertexBuffer(TWT::uint StartSlot, TW3DVertexBuffer* VertexBuffer) {
	command_list->IASetVertexBuffers(StartSlot, 1, &VertexBuffer->GetView());
}

void TW3DGraphicsCommandList::SetVertexBuffers(TWT::uint StartSlot, const std::vector<D3D12_VERTEX_BUFFER_VIEW>& views) {
	command_list->IASetVertexBuffers(StartSlot, static_cast<UINT>(views.size()), views.data());
}

void TW3DGraphicsCommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view) {
	command_list->IASetIndexBuffer(view);
}

void TW3DGraphicsCommandList::Draw(TWT::uint VertexCountPerInstance, TWT::uint StartVertexLocation, TWT::uint InstanceCount, TWT::uint StartInstanceLocation) {
	command_list->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void TW3DGraphicsCommandList::DrawIndexed(TWT::uint IndexCountPerInstance, TWT::uint StartIndexLocation, TWT::uint InstanceCount, TWT::uint StartInstanceLocation, int BaseVertexLocation) {
	command_list->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void TW3DGraphicsCommandList::Dispatch(TWT::uint ThreadGroupCountX, TWT::uint ThreadGroupCountY, TWT::uint ThreadGroupCountZ) {
	command_list->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void TW3DGraphicsCommandList::ExecuteIndirect(ID3D12CommandSignature* CommandSignature, TWT::uint MaxCommandCount, ID3D12Resource* ArgumentBuffer,
	TWT::uint64 ArgumentBufferOffset, ID3D12Resource* CountBuffer, TWT::uint64 CountBufferOffset)
{
	command_list->ExecuteIndirect(CommandSignature, MaxCommandCount, ArgumentBuffer, ArgumentBufferOffset, CountBuffer, CountBufferOffset);
}

void TW3DGraphicsCommandList::BindResources(TW3DResourceManager* ResourceManager) {
	SetDescriptorHeap(ResourceManager->GetSVDescriptorHeap());
}

void TW3DGraphicsCommandList::BindRenderTargetTexture(TWT::uint RootParameterIndex, TW3DRenderTarget* RenderTarget) {
	command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, RenderTarget->GetSRVGPUHandle());
}

void TW3DGraphicsCommandList::BindBuffer(TWT::uint RootParameterIndex, TW3DResource* Resource, bool UAV) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE) {
		if (UAV)
			command_list->SetComputeRootUnorderedAccessView(RootParameterIndex, Resource->GetGPUVirtualAddress());
		else
			command_list->SetComputeRootShaderResourceView(RootParameterIndex, Resource->GetGPUVirtualAddress());
	} else {
		if (UAV)
			command_list->SetGraphicsRootUnorderedAccessView(RootParameterIndex, Resource->GetGPUVirtualAddress());
		else
			command_list->SetGraphicsRootShaderResourceView(RootParameterIndex, Resource->GetGPUVirtualAddress());
	}
}

void TW3DGraphicsCommandList::BindTexture(TWT::uint RootParameterIndex, TW3DTexture* Resource, bool UAV) {
	if (UAV)
		SetRootDescriptorTable(RootParameterIndex, Resource->GetGPUUAVHandle());
	else
		SetRootDescriptorTable(RootParameterIndex, Resource->GetGPUSRVHandle());
}

void TW3DGraphicsCommandList::BindConstantBuffer(TWT::uint RootParameterIndex, TW3DConstantBuffer* CB, TWT::uint ElementIndex) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootConstantBufferView(RootParameterIndex, CB->GetAddress(ElementIndex));
	else
		command_list->SetGraphicsRootConstantBufferView(RootParameterIndex, CB->GetAddress(ElementIndex));
}

void TW3DGraphicsCommandList::BindCameraCBV(TWT::uint RootParameterIndex, TW3DPerspectiveCamera* Camera) {
	BindConstantBuffer(RootParameterIndex, Camera->GetConstantBuffer());
}

void TW3DGraphicsCommandList::DrawObject(TW3DObject* object, TWT::uint ModelCBRootParameterIndex) {
	object->RecordDraw(this, ModelCBRootParameterIndex);
}

void TW3DGraphicsCommandList::Reset() {
	TWU::SuccessAssert(command_allocator->Reset(), "TW3DGraphicsCommandList::Reset, command_allocator->Reset"s);
	TWU::SuccessAssert(command_list->Reset(command_allocator, nullptr), "TW3DGraphicsCommandList::Reset, command_list->Reset"s);
}

void TW3DGraphicsCommandList::Close() {
	TWU::SuccessAssert(command_list->Close(), "TW3DGraphicsCommandList::Close"s);
	empty = false;
}

void TW3DGraphicsCommandList::EmptyReset() {
	Reset();
	Close();
	empty = true;
}

bool TW3DGraphicsCommandList::IsEmpty() {
	return empty;
}

TW3DGraphicsCommandList* TW3DGraphicsCommandList::CreateDirect(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3DGraphicsCommandList* TW3DGraphicsCommandList::CreateBundle(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_BUNDLE);
}

TW3DGraphicsCommandList* TW3DGraphicsCommandList::CreateCompute(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}

TW3DGraphicsCommandList* TW3DGraphicsCommandList::CreateCopy(TW3DDevice* device) {
	return new TW3DGraphicsCommandList(device, D3D12_COMMAND_LIST_TYPE_COPY);
}

D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = Resource ? Resource->Get() : nullptr;
	return barrier;
}