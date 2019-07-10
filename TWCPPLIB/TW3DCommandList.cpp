#include "pch.h"
#include "TW3DCommandList.h"
#include "TW3DResourceManager.h"
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"
#include "TW3DGraphicsPipeline.h"
#include "TW3DCommandBuffer.h"

TW3DCommandList::TW3DCommandList(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) :
	type(Type) {

	Device->CreateCommandAllocator(Type, &command_allocator);
	Device->CreateGraphicsCommandList(Type, command_allocator, nullptr, &Native);
	Native->SetName(L"TW3DCommandList");
	command_allocator->SetName(L"TW3D ID3D12CommandAllocator");
	Native->Close();
}

TW3DCommandList::TW3DCommandList(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type, TW3DGraphicsPipeline* InitialState) :
	type(Type), initial_pipeline_state(InitialState->Native) {

	Device->CreateCommandAllocator(Type, &command_allocator);
	Device->CreateGraphicsCommandList(Type, command_allocator, InitialState->Native, &Native);
	Native->SetName(L"TW3DCommandList");
	command_allocator->SetName(L"TW3D ID3D12CommandAllocator");
	Native->Close();
}

TW3DCommandList::TW3DCommandList(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type, TW3DComputePipeline* InitialState) :
	type(Type), initial_pipeline_state(InitialState->Native) {

	Device->CreateCommandAllocator(Type, &command_allocator);
	Device->CreateGraphicsCommandList(Type, command_allocator, InitialState->Native, &Native);
	Native->SetName(L"TW3DCommandList");
	command_allocator->SetName(L"TW3D ID3D12CommandAllocator");
	Native->Close();
}

TW3DCommandList::~TW3DCommandList() {
	TWU::DXSafeRelease(Native);
	TWU::DXSafeRelease(command_allocator);
}

D3D12_COMMAND_LIST_TYPE TW3DCommandList::GetType() {
	return type;
}

void TW3DCommandList::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
	Native->ResourceBarrier(1, &barrier);
}

void TW3DCommandList::ResourceBarriers(const std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	Native->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
}

void TW3DCommandList::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	Native->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource->Native, StateBefore, StateAfter));
}

void TW3DCommandList::ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	Native->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Resource, StateBefore, StateAfter));
}

void TW3DCommandList::CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount) {
	Native->CopyBufferRegion(DstBuffer->Native, DstOffset, SrcBuffer->Native, SrcOffset, ByteCount);
}

void TW3DCommandList::CopyTextureRegion(TW3DTexture* DstTexture, TW3DTexture* SrcTexture) {
	Native->CopyTextureRegion(&CD3DX12_TEXTURE_COPY_LOCATION(DstTexture->Native), 0, 0, 0, &CD3DX12_TEXTURE_COPY_LOCATION(SrcTexture->Native), nullptr);
}

void TW3DCommandList::CopyTextureRegion(D3D12_TEXTURE_COPY_LOCATION const* Dst, TWT::uint3 DstXYZ, D3D12_TEXTURE_COPY_LOCATION const* Src, D3D12_BOX const* SrcBox) {
	Native->CopyTextureRegion(Dst, DstXYZ.x, DstXYZ.y, DstXYZ.z, Src, SrcBox);
}

void TW3DCommandList::SetPipeline(TW3DGraphicsPipeline* GraphicsPipeline) {
	Native->SetPipelineState(GraphicsPipeline->Native);
	SetRootSignatureFrom(GraphicsPipeline);
}

void TW3DCommandList::SetPipeline(TW3DComputePipeline* ComputePipeline) {
	Native->SetPipelineState(ComputePipeline->Native);
	SetRootSignatureFrom(ComputePipeline);
}

void TW3DCommandList::SetRenderTarget(TW3DRenderTarget* RenderTarget, TW3DTexture* DSV) {
	Native->OMSetRenderTargets(1, &RenderTarget->GetCPURTVHandle(), false, DSV ? &DSV->GetCPUDSVHandle() : nullptr);
}

void TW3DCommandList::SetRenderTargets(const std::vector<TW3DRenderTarget*>& RTVs, TW3DTexture* DSV) {
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles(RTVs.size());
	for (int i = 0; i < RTVs.size(); i++)
		handles[i] = RTVs[i]->GetCPURTVHandle();
	Native->OMSetRenderTargets(static_cast<UINT>(RTVs.size()), handles.data(), false, DSV ? &DSV->GetCPUDSVHandle() : nullptr);
}

void TW3DCommandList::ClearRTV(TW3DRenderTarget* RenderTarget) {
	Native->ClearRenderTargetView(RenderTarget->GetCPURTVHandle(), RenderTarget->GetClearValue().Color, 0, nullptr);
}

void TW3DCommandList::ClearRTV(TW3DRenderTarget* RenderTarget, TWT::float4 Color) {
	float clearV[] = { Color.x, Color.y, Color.z, Color.w };
	Native->ClearRenderTargetView(RenderTarget->GetCPURTVHandle(), clearV, 0, nullptr);
}

void TW3DCommandList::ClearDSVDepth(TW3DTexture* Texture) {
	auto clear_value = Texture->GetClearValue();
	Native->ClearDepthStencilView(Texture->GetCPUDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, clear_value.DepthStencil.Depth, clear_value.DepthStencil.Stencil, 0, nullptr);
}

void TW3DCommandList::SetRootSignature(TW3DRootSignature* RootSignature) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		Native->SetComputeRootSignature(RootSignature->Native);
	else
		Native->SetGraphicsRootSignature(RootSignature->Native);
}

void TW3DCommandList::SetRootSignatureFrom(TW3DGraphicsPipeline* GraphicsPipeline) {
	Native->SetGraphicsRootSignature(GraphicsPipeline->RootSignature->Native);
}

void TW3DCommandList::SetRootSignatureFrom(TW3DComputePipeline* ComputePipeline) {
	Native->SetComputeRootSignature(ComputePipeline->RootSignature->Native);
}

void TW3DCommandList::SetDescriptorHeap(TW3DDescriptorHeap* heap) {
	ID3D12DescriptorHeap* nativeHeap = heap->Get();
	Native->SetDescriptorHeaps(1, &nativeHeap);
}

void TW3DCommandList::SetDescriptorHeaps(std::vector<TW3DDescriptorHeap*> heaps) {
	std::vector<ID3D12DescriptorHeap*> nativeHeaps(heaps.size());

	for (TWT::uint i = 0; i < nativeHeaps.size(); i++)
		nativeHeaps[i] = heaps[i]->Get();

	Native->SetDescriptorHeaps(static_cast<UINT>(heaps.size()), nativeHeaps.data());
}

void TW3DCommandList::SetRootDescriptorTable(TWT::uint RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		Native->SetComputeRootDescriptorTable(RootParameterIndex, BaseDescriptor);
	else
		Native->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}

void TW3DCommandList::BindUIntConstant(TWT::uint RootParameterIndex, TWT::uint Data, TWT::uint DestOffsetIn32BitValues) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		Native->SetComputeRoot32BitConstant(RootParameterIndex, Data, DestOffsetIn32BitValues);
	else
		Native->SetGraphicsRoot32BitConstant(RootParameterIndex, Data, DestOffsetIn32BitValues);
}

void TW3DCommandList::BindFloatConstant(TWT::uint RootParameterIndex, float Data, TWT::uint DestOffsetIn32BitValues) {
	BindUIntConstant(RootParameterIndex, *(TWT::uint*)&Data, DestOffsetIn32BitValues);
}

void TW3DCommandList::BindUIntConstants(TWT::uint RootParameterIndex, std::vector<TWT::uint> const& Num32BitValues, TWT::uint DestOffsetIn32BitValues) {
	if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		Native->SetComputeRoot32BitConstants(RootParameterIndex, Num32BitValues.size(), Num32BitValues.data(), DestOffsetIn32BitValues);
	else
		Native->SetGraphicsRoot32BitConstants(RootParameterIndex, Num32BitValues.size(), Num32BitValues.data(), DestOffsetIn32BitValues);
}

void TW3DCommandList::SetViewport(const D3D12_VIEWPORT* viewport) {
	Native->RSSetViewports(1, viewport);
}

void TW3DCommandList::SetScissor(const D3D12_RECT* scissor) {
	Native->RSSetScissorRects(1, scissor);
}

void TW3DCommandList::SetViewportAndScissor(TWT::uint2 Size) {
	Native->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f, Size.x, Size.y));
	Native->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, Size.x, Size.y));
}

void TW3DCommandList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) {
	Native->IASetPrimitiveTopology(PrimitiveTopology);
}

void TW3DCommandList::SetVertexBuffer(TWT::uint StartSlot, TW3DVertexBuffer* VertexBuffer) {
	Native->IASetVertexBuffers(StartSlot, 1, &VertexBuffer->GetView());
}

void TW3DCommandList::SetVertexBuffers(TWT::uint StartSlot, const std::vector<D3D12_VERTEX_BUFFER_VIEW>& views) {
	Native->IASetVertexBuffers(StartSlot, static_cast<UINT>(views.size()), views.data());
}

void TW3DCommandList::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view) {
	Native->IASetIndexBuffer(view);
}

void TW3DCommandList::Draw(TWT::uint VertexCountPerInstance, TWT::uint StartVertexLocation, TWT::uint InstanceCount, TWT::uint StartInstanceLocation) {
	Native->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void TW3DCommandList::DrawIndexed(TWT::uint IndexCountPerInstance, TWT::uint StartIndexLocation, TWT::uint InstanceCount, TWT::uint StartInstanceLocation, int BaseVertexLocation) {
	Native->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void TW3DCommandList::Dispatch(TWT::uint ThreadGroupCountX, TWT::uint ThreadGroupCountY, TWT::uint ThreadGroupCountZ) {
	Native->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}

void TW3DCommandList::Dispatch(TWT::uint2 ThreadGroupCountXY, TWT::uint ThreadGroupCountZ) {
	Native->Dispatch(ThreadGroupCountXY.x, ThreadGroupCountXY.y, ThreadGroupCountZ);
}

void TW3DCommandList::ExecuteIndirect(ID3D12CommandSignature* CommandSignature, TWT::uint MaxCommandCount, ID3D12Resource* ArgumentBuffer,
	TWT::uint64 ArgumentBufferOffset, ID3D12Resource* CountBuffer, TWT::uint64 CountBufferOffset) {
	Native->ExecuteIndirect(CommandSignature, MaxCommandCount, ArgumentBuffer, ArgumentBufferOffset, CountBuffer, CountBufferOffset);
}

void TW3DCommandList::ExecuteIndirect(TW3DCommandBuffer* CommandBuffer) {
	ExecuteIndirect(CommandBuffer->GetSignature()->Native, CommandBuffer->GetMaxCommandCount(), CommandBuffer->GetCommandBuffer()->Native, 0, CommandBuffer->GetCountBuffer()->Native);
}

void TW3DCommandList::ExecuteBundle(TW3DCommandList* CommandList) {
	Native->ExecuteBundle(CommandList->Native);
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
			Native->SetComputeRootUnorderedAccessView(RootParameterIndex, Resource->GetGPUVirtualAddress());
		else
			Native->SetComputeRootShaderResourceView(RootParameterIndex, Resource->GetGPUVirtualAddress());
	} else {
		if (UAV)
			Native->SetGraphicsRootUnorderedAccessView(RootParameterIndex, Resource->GetGPUVirtualAddress());
		else
			Native->SetGraphicsRootShaderResourceView(RootParameterIndex, Resource->GetGPUVirtualAddress());
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
		Native->SetComputeRootConstantBufferView(RootParameterIndex, CB->GetGPUVirtualAddress(ElementIndex));
	else
		Native->SetGraphicsRootConstantBufferView(RootParameterIndex, CB->GetGPUVirtualAddress(ElementIndex));
}

void TW3DCommandList::BindCameraCBV(TWT::uint RootParameterIndex, TW3DPerspectiveCamera* Camera) {
	BindConstantBuffer(RootParameterIndex, Camera->GetConstantBuffer());
}

void TW3DCommandList::BindCameraPrevCBV(TWT::uint RootParameterIndex, TW3DPerspectiveCamera* Camera) {
	BindConstantBuffer(RootParameterIndex, Camera->GetPreviousConstantBuffer());
}

void TW3DCommandList::ClearTexture(TW3DTexture* Texture, TWT::float4 Color) {
	float value[] = { Color.r, Color.b, Color.g, Color.a };
	Native->ClearUnorderedAccessViewFloat(Texture->GetGPUCPUUAVHandle(), Texture->GetCPUUAVHandle(), Texture->Native, value, 0, nullptr);
}

void TW3DCommandList::DebugBeginEvent(TWT::String const& Name, TWT::float3 Color) {
	PIXBeginEvent(Native, PIX_COLOR(Color.r * 255, Color.g * 255, Color.b * 255), Name.ToCharArray());
}

void TW3DCommandList::DebugEndEvent() {
	PIXEndEvent();
}

void TW3DCommandList::Reset() {
	empty = true;
	
	TWU::SuccessAssert(command_allocator->Reset(), "TW3DCommandList::Reset, command_allocator->Reset"s);
	TWU::SuccessAssert(Native->Reset(command_allocator, initial_pipeline_state), "TW3DCommandList::Reset, Native->Reset"s);
}

void TW3DCommandList::Close() {
	TWU::SuccessAssert(Native->Close(), "TW3DCommandList::Close"s);
	empty = false;
}

bool TW3DCommandList::IsEmpty() {
	return empty;
}


D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = Resource ? Resource->Native : nullptr;
	return barrier;
}