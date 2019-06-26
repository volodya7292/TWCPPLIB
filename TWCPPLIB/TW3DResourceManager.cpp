#include "pch.h"
#include "TW3DResourceManager.h"

TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device)
{
	temp_gcl = new TW3DTempGCL(device);
	rtv_descriptor_heap = TW3DDescriptorHeap::CreateForRTV(device, 16);
	dsv_descriptor_heap = TW3DDescriptorHeap::CreateForDSV(device, 4);
	srv_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 128);
	uav_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 128, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	direct_command_queue = TW3DCommandQueue::CreateDirect(device);
	compute_command_queue = TW3DCommandQueue::CreateCompute(device);
	copy_command_queue = TW3DCommandQueue::CreateCopy(device);
	temp_direct_cl = CreateDirectCommandList();
	temp_compute_cl = CreateComputeCommandList();
	temp_copy_cl = CreateCopyCommandList();
}

TW3DResourceManager::~TW3DResourceManager() {
	delete temp_gcl;
	delete temp_direct_cl;
	delete temp_compute_cl;
	delete temp_copy_cl;
	delete rtv_descriptor_heap;
	delete dsv_descriptor_heap;
	delete srv_descriptor_heap;
	delete uav_descriptor_heap;
	delete direct_command_queue;
	delete compute_command_queue;
	delete copy_command_queue;
}

TW3DFramebuffer* TW3DResourceManager::CreateFramebuffer(TWT::uint2 Size) {
	return new TW3DFramebuffer(this, Size);
}

TW3DRenderTarget* TW3DResourceManager::CreateRenderTarget(ID3D12Resource* Buffer) {
	auto* rtv = new TW3DRenderTarget(device, rtv_descriptor_heap);
	rtv->Create(Buffer);
	return rtv;
}

TW3DRenderTarget* TW3DResourceManager::CreateRenderTarget(TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	auto* rtv = new TW3DRenderTarget(device, rtv_descriptor_heap, srv_descriptor_heap, Format, ClearValue);
	rtv->Create(Size);
	return rtv;
}

TW3DBuffer* TW3DResourceManager::CreateBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes, bool UAV) {
	auto* buffer = new TW3DBuffer(device, temp_gcl, false, ElementSizeInBytes, UAV, srv_descriptor_heap);
	buffer->Create(ElementCount);
	return buffer;
}

TW3DVertexBuffer* TW3DResourceManager::CreateVertexBuffer(TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes, bool OptimizeForUpdating) {
	return new TW3DVertexBuffer(device, temp_gcl, OptimizeForUpdating, VertexCount, SingleVertexSizeInBytes);
}

TW3DConstantBuffer* TW3DResourceManager::CreateConstantBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes) {
	return new TW3DConstantBuffer(device, ElementCount, ElementSizeInBytes);
}

TW3DTexture* TW3DResourceManager::CreateDepthStencilTexture(TWT::uint2 Size) {
	auto* texture = new TW3DTexture(device, temp_gcl, DXGI_FORMAT_D32_FLOAT, nullptr, nullptr, dsv_descriptor_heap);
	texture->CreateDepthStencil(Size);
	return texture;
}

TW3DTexture* TW3DResourceManager::CreateTexture2D(const TWT::WString& Filename) {
	return TW3DTexture::Create2D(device, temp_gcl, srv_descriptor_heap, Filename);
}

TW3DTexture* TW3DResourceManager::CreateTexture2D(TWT::uint2 Size, DXGI_FORMAT Format, bool UAV) {
	auto* texture = new TW3DTexture(device, temp_gcl, Format, srv_descriptor_heap, UAV ? uav_descriptor_heap : nullptr, nullptr);
	texture->Create2D(Size);
	return texture;
}

TW3DTexture* TW3DResourceManager::CreateTextureArray2D(TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV) {
	auto* texture = new TW3DTexture(device, temp_gcl, Format, srv_descriptor_heap, UAV ? uav_descriptor_heap : nullptr, nullptr);
	texture->CreateArray2D(Size, Depth);
	return texture;
}

TW3DCommandList* TW3DResourceManager::CreateDirectCommandList() {
	return TW3DCommandList::CreateDirect(device);
}

TW3DCommandList* TW3DResourceManager::CreateBundleCommandList() {
	return TW3DCommandList::CreateBundle(device);
}

TW3DCommandList* TW3DResourceManager::CreateComputeCommandList() {
	return TW3DCommandList::CreateCompute(device);
}

TW3DCommandList* TW3DResourceManager::CreateCopyCommandList() {
	return TW3DCommandList::CreateCopy(device);
}

TW3DCommandList* TW3DResourceManager::GetTemporaryDirectCommandList() {
	temp_direct_cl->Reset();
	temp_direct_cl->BindResources(this);
	return temp_direct_cl;
}

TW3DCommandList* TW3DResourceManager::GetTemporaryComputeCommandList() {
	temp_compute_cl->Reset();
	temp_compute_cl->BindResources(this);
	return temp_compute_cl;
}

TW3DCommandList* TW3DResourceManager::GetTemporaryCopyCommandList() {
	temp_copy_cl->Reset();
	return temp_copy_cl;
}

TW3DCommandQueue* TW3DResourceManager::GetCommandListCommandQueue(TW3DCommandList* CommandList) {
	switch (CommandList->GetType()) {
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		return compute_command_queue;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		return copy_command_queue;
	default:
		return direct_command_queue;
	}
}

void TW3DResourceManager::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(Resource->Get(), StateBefore, StateAfter);
	temp_gcl->Execute();
}

bool TW3DResourceManager::IsCommandListRunning(TW3DCommandList* CommandList) {
	return GetCommandListCommandQueue(CommandList)->IsCommandListRunning(CommandList);
}

void TW3DResourceManager::FlushCommandList(TW3DCommandList* CommandList) {
	GetCommandListCommandQueue(CommandList)->FlushCommandList(CommandList);
}

void TW3DResourceManager::FlushCommandLists() {
	copy_command_queue->FlushCommands();
	compute_command_queue->FlushCommands();
	direct_command_queue->FlushCommands();
	
}

void TW3DResourceManager::ExecuteCommandList(TW3DCommandList* CommandList) {
	GetCommandListCommandQueue(CommandList)->ExecuteCommandList(CommandList);
}

void TW3DResourceManager::ExecuteCommandLists(const std::vector<TW3DCommandList*>& CommandLists) {
	GetCommandListCommandQueue(CommandLists[0])->ExecuteCommandLists(CommandLists);
}

TW3DDevice* TW3DResourceManager::GetDevice() {
	return device;
}

TW3DDescriptorHeap* TW3DResourceManager::GetSVDescriptorHeap() {
	return srv_descriptor_heap;
}

TW3DCommandQueue* TW3DResourceManager::GetDirectCommandQueue() {
	return direct_command_queue;
}
