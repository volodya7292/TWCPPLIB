#include "pch.h"
#include "TW3DCommandList.h"
#include "TW3DResourceManager.h"
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"

TW3DCommandList::TW3DCommandList(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) :
	type(Type)
{
	Device->CreateCommandAllocator(Type, &command_allocator);
	Device->CreateGraphicsCommandList(Type, command_allocator, &command_list);
	command_list->SetName(L"TW3DCommandList");
	command_allocator->SetName(L"TW3D ID3D12CommandAllocator");
	command_list->Close();
}

TW3DCommandList::~TW3DCommandList() {
	TWU::DXSafeRelease(command_list);
	TWU::DXSafeRelease(command_allocator);
}

ID3D12GraphicsCommandList* TW3DCommandList::Get() {
	return command_list;
}

D3D12_COMMAND_LIST_TYPE TW3DCommandList::GetType() {
	return type;
}

void TW3DCommandList::UpdateSubresources(TW3DResource* DestinationResource, TW3DResource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::uint SubresourcesCount, TWT::uint64 IntermediateOffset, TWT::uint FirstSubresource) {
	TWU::UpdateSubresourcesImp(command_list, DestinationResource->Get(), Intermediate->Get(), SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3DCommandList::UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate,
	D3D12_SUBRESOURCE_DATA* SrcData, TWT::uint SubresourcesCount, TWT::uint64 IntermediateOffset, TWT::uint FirstSubresource) {
	TWU::UpdateSubresourcesImp(command_list, DestinationResource, Intermediate, SrcData, SubresourcesCount, IntermediateOffset, FirstSubresource);
}

void TW3DCommandList::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
	command_list->ResourceBarrier(1, &barrier);
}

void TW3DCommandList::ResourceBarriers(const std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	command_list->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void TW3DCommandList::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->Get(), StateBefore, StateAfter));
}

void TW3DCommandList::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, StateBefore, StateAfter));
}

void TW3DCommandList::CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount) {
	command_list->CopyBufferRegion(DstBuffer->Get(), DstOffset, SrcBuffer->Get(), SrcOffset, ByteCount);
}

void TW3DCommandList::CopyTextureRegion(TW3DResource* DstTexture, TW3DResource* SrcTexture) {
	command_list->CopyTextureRegion(&CD3DX12_TEXTURE_COPY_LOCATION(DstTexture->Get()), 0, 0, 0, &CD3DX12_TEXTURE_COPY_LOCATION(SrcTexture->Get()), nullptr);
}

void TW3DCommandList::SetPipelineState(TW3DGraphicsPipelineState* PipelineState) {
	command_list->SetPipelineState(PipelineState->Get());
	SetRootSignature(PipelineState->RootSignature);
}

void TW3DCommandList::SetPipelineState(TW3DComputePipelineState* PipelineState) {
	command_list->SetPipelineState(PipelineState->Get());
	SetRootSignature(PipelineState->RootSignature);
}

void TW3DCommandList::SetRenderTarget(TW3DRenderTarget* RenderTarget, TW3DTexture* DSV) {
	command_list->OMSetRenderTargets(1, &RenderTarget->GetCPURTVHandle(), false, DSV ? &DSV->GetCPUDSVHandle() : nullptr);
}

void TW3DCommandList::SetRenderTargets(const std::vector<TW3DRenderTarget*>& RTVs, TW3DTexture* DSV) {
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(RTVs.size());
	for (int i = 0; i < RTVs.size(); i++)
		handles[i] = RTVs[i]->GetCPURTVHandle();
	command_list->OMSetRenderTargets(static_cast<UINT>(RTVs.size()), handles.data(), false, DSV ? &DSV->GetCPUDSVHandle() : nullptr);
}

void TW3DCommandList::ClearRTV(TW3DRenderTarget* RenderTarget) {
	command_list->ClearRenderTargetView(RenderTarget->GetCPURTVHandle(), RenderTarget->GetClearValue().Color, 0, nullptr);
}

void TW3DCommandList::ClearRTV(TW3DRenderTarget* RenderTarget, TWT::float4 Color) {
	float clearV[] = { Color.x, Color.y, Color.z, Color.w };
	command_list->ClearRenderTargetView(RenderTarget->GetCPURTVHandle(), clearV, 0, nullptr);
}

void TW3DCommandList::ClearDSVDepth(TW3DTexture* Texture) {
	auto clear_value = Texture->GetClearValue();
	command_list->ClearDepthStencilView(Texture->GetCPUDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, clear_value.DepthStencil.Depth, clear_value.DepthStencil.Stencil, 0, nullptr);
}

void TW3DCommandList::SetRootSignature(TW3DRootSignature* RootSignature) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootSignature(RootSignature->Get());
	else
		command_list->SetGraphicsRootSignature(RootSignature->Get());
}

void TW3DCommandList::SetDescriptorHeap(TW3DDescriptorHeap* heap) {
	ID3D12DescriptorHeap* nativeHeap = heap->Get();
	command_list->SetDescriptorHeaps(1, &nativeHeap);
}

void TW3DCommandList::SetDescriptorHeaps(std::vector<TW3DDescriptorHeap*> heaps) {
	std::vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());

	for (TWT::uint i = 0; i < nativeHeaps.size(); i++)
		nativeHeaps[i] = heaps[i]->Get();

	command_list->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
}

void TW3DCommandList::SetRootDescriptorTable(TWT::uint RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootDescriptorTable(RootParameterIndex, BaseDescriptor);
	else
		command_list->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}

void TW3DCommandList::BindUIntConstant(TWT::uint RootParameterIndex, TWT::uint Data, TWT::uint DestOffsetIn32BitValues) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRoot32BitConstant(RootParameterIndex, Data, DestOffsetIn32BitValues);
	else
		command_list->SetGraphicsRoot32BitConstant(RootParameterIndex, Data, DestOffsetIn32BitValues);
}

void TW3DCommandList::BindFloatConstant(TWT::uint RootParameterIndex, float Data, TWT::uint DestOffsetIn32BitValues) {
	BindUIntConstant(RootParameterIndex, *(TWT::uint*)&Data, DestOffsetIn32BitValues);
}

void TW3DCommandList::BindUIntConstants(TWT::uint RootParameterIndex, TWT::uint Num32BitValuesToSet, const void* Data, TWT::uint DestOffsetIn32BitValues) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, Data, DestOffsetIn32BitValues);
	else
		command_list->SetGraphicsRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, Data, DestOffsetIn32BitValues);
}

void TW3DCommandList::SetViewport(const D3D12_VIEWPORT* viewport) {
	command_list->RSSetViewports(1, viewport);
}

void TW3DCommandList::SetScissor(const D3D12_RECT* scissor) {
	command_list->RSSetScissorRects(1, scissor);
}

void TW3DCommandList::SetViewportScissor(TWT::uint2 Size) {
	command_list->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, Size.x, Size.y));
	command_list->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, Size.x, Size.y));
}

void TW3DCommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) {
	command_list->IASetPrimitiveTopology(PrimitiveTopology);
}

void TW3DCommandList::SetVertexBuffer(TWT::uint StartSlot, TW3DVertexBuffer* VertexBuffer) {
	command_list->IASetVertexBuffers(StartSlot, 1, &VertexBuffer->GetView());
}

void TW3DCommandList::SetVertexBuffers(TWT::uint StartSlot, const std::vector<D3D12_VERTEX_BUFFER_VIEW>& views) {
	command_list->IASetVertexBuffers(StartSlot, static_cast<UINT>(views.size()), views.data());
}

void TW3DCommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view) {
	command_list->IASetIndexBuffer(view);
}

void TW3DCommandList::Draw(TWT::uint VertexCountPerInstance, TWT::uint StartVertexLocation, TWT::uint InstanceCount, TWT::uint StartInstanceLocation) {
	command_list->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void TW3DCommandList::DrawIndexed(TWT::uint IndexCountPerInstance, TWT::uint StartIndexLocation, TWT::uint InstanceCount, TWT::uint StartInstanceLocation, int BaseVertexLocation) {
	command_list->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void TW3DCommandList::Dispatch(TWT::uint ThreadGroupCountX, TWT::uint ThreadGroupCountY, TWT::uint ThreadGroupCountZ) {
	command_list->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void TW3DCommandList::Dispatch(TWT::uint2 ThreadGroupCountXY, TWT::uint ThreadGroupCountZ) {
	command_list->Dispatch(ThreadGroupCountXY.x, ThreadGroupCountXY.y, ThreadGroupCountZ);
}

void TW3DCommandList::ExecuteIndirect(ID3D12CommandSignature* CommandSignature, TWT::uint MaxCommandCount, ID3D12Resource* ArgumentBuffer,
	TWT::uint64 ArgumentBufferOffset, ID3D12Resource* CountBuffer, TWT::uint64 CountBufferOffset)
{
	command_list->ExecuteIndirect(CommandSignature, MaxCommandCount, ArgumentBuffer, ArgumentBufferOffset, CountBuffer, CountBufferOffset);
}

void TW3DCommandList::DrawQuad() {
	SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Draw(4);
}

void TW3DCommandList::BindResources(TW3DResourceManager* ResourceManager) {
	SetDescriptorHeap(ResourceManager->GetSVDescriptorHeap());
}

void TW3DCommandList::BindFramebuffer(TW3DFramebuffer* Framebuffer) {
	SetRenderTargets(Framebuffer->GetRenderTargets(), Framebuffer->GetDepthStencilTexture());
}

void TW3DCommandList::BindBuffer(TWT::uint RootParameterIndex, TW3DResource* Resource, bool UAV) {
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

void TW3DCommandList::BindTexture(TWT::uint RootParameterIndex, TW3DTexture* Texture, bool UAV) {
	SetRootDescriptorTable(RootParameterIndex, UAV ? Texture->GetGPUUAVHandle() : Texture->GetGPUSRVHandle());
}

void TW3DCommandList::BindTexture(TWT::uint RootParameterIndex, TW3DRenderTarget* RenderTarget) {
	SetRootDescriptorTable(RootParameterIndex, RenderTarget->GetGPUSRVHandle());
}

void TW3DCommandList::BindConstantBuffer(TWT::uint RootParameterIndex, TW3DConstantBuffer* CB, TWT::uint ElementIndex) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_list->SetComputeRootConstantBufferView(RootParameterIndex, CB->GetAddress(ElementIndex));
	else
		command_list->SetGraphicsRootConstantBufferView(RootParameterIndex, CB->GetAddress(ElementIndex));
}

void TW3DCommandList::BindCameraCBV(TWT::uint RootParameterIndex, TW3DPerspectiveCamera* Camera) {
	BindConstantBuffer(RootParameterIndex, Camera->GetConstantBuffer());
}

void TW3DCommandList::BindCameraPrevCBV(TWT::uint RootParameterIndex, TW3DPerspectiveCamera* Camera) {
	BindConstantBuffer(RootParameterIndex, Camera->GetPreviousConstantBuffer());
}

void TW3DCommandList::ClearTexture(TW3DTexture* Texture, TWT::float4 Color) {
	float value[] = { Color.r, Color.b, Color.g, Color.a };
	command_list->ClearUnorderedAccessViewFloat(Texture->GetGPUCPUUAVHandle(), Texture->GetCPUUAVHandle(), Texture->Get(), value, 0, nullptr);
}

void TW3DCommandList::DrawObject(TW3DObject* object, TWT::uint ModelCBRootParameterIndex) {
	object->RecordDraw(this, ModelCBRootParameterIndex);
}

void TW3DCommandList::Reset() {
	empty = true;
	TWU::SuccessAssert(command_allocator->Reset(), "TW3DCommandList::Reset, command_allocator->Reset"s);
	TWU::SuccessAssert(command_list->Reset(command_allocator, nullptr), "TW3DCommandList::Reset, command_list->Reset"s);
}

void TW3DCommandList::Close() {
	TWU::SuccessAssert(command_list->Close(), "TW3DCommandList::Close"s);
	empty = false;
}

bool TW3DCommandList::IsEmpty() {
	return empty;
}

TW3DCommandList* TW3DCommandList::CreateDirect(TW3DDevice* device) {
	return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3DCommandList* TW3DCommandList::CreateBundle(TW3DDevice* device) {
	return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_BUNDLE);
}

TW3DCommandList* TW3DCommandList::CreateCompute(TW3DDevice* device) {
	return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}

TW3DCommandList* TW3DCommandList::CreateCopy(TW3DDevice* device) {
	return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COPY);
}

D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = Resource ? Resource->Get() : nullptr;
	return barrier;
}