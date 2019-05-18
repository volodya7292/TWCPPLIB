#include "pch.h"
#include "TW3DResourceManager.h"

TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device)
{
	temp_gcl = new TW3DTempGCL(device);
	rtv_descriptor_heap = TW3DDescriptorHeap::CreateForRTV(device, 1024);
	dsv_descriptor_heap = TW3DDescriptorHeap::CreateForDSV(device, 1024);
	srv_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 1024);
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
	delete direct_command_queue;
	delete compute_command_queue;
	delete copy_command_queue;
}

TW3DResourceRTV* TW3DResourceManager::CreateRenderTargetView(ID3D12Resource* Buffer) {
	TW3DResourceRTV* rtv = new TW3DResourceRTV(device, rtv_descriptor_heap);
	rtv->Create(Buffer);
	return rtv;
}

TW3DResourceRTV* TW3DResourceManager::CreateRenderTargetView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Vector4f ClearValue) {
	TW3DResourceRTV* rtv = new TW3DResourceRTV(device, rtv_descriptor_heap, srv_descriptor_heap, Format, ClearValue);
	rtv->Create(Width, Height);
	return rtv;
}

TW3DResourceDSV* TW3DResourceManager::CreateDepthStencilView(TWT::UInt Width, TWT::UInt Height) {
	TW3DResourceDSV* dsv = new TW3DResourceDSV(device, dsv_descriptor_heap);
	dsv->Create(Width, Height);
	return dsv;
}

TW3DResourceUAV* TW3DResourceManager::CreateUnorderedAccessView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format) {
	TW3DResourceUAV* uav = new TW3DResourceUAV(device, srv_descriptor_heap, Format);
	uav->CreateTexture2D(Width, Height);
	return uav;
}

TW3DResourceUAV* TW3DResourceManager::CreateUnorderedAccessView(TWT::UInt ElementCount, TWT::UInt ElementSizeInBytes) {
	TW3DResourceUAV* uav = new TW3DResourceUAV(device, srv_descriptor_heap, ElementSizeInBytes, temp_gcl);
	uav->CreateBuffer(ElementCount);
	return uav;
}

TW3DVertexBuffer* TW3DResourceManager::CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes) {
	return new TW3DVertexBuffer(device, VertexCount, SingleVertexSizeInBytes, temp_gcl);
}

TW3DResourceCB* TW3DResourceManager::CreateConstantBuffer(TWT::UInt ElementCount, TWT::UInt ElementSizeInBytes) {
	return new TW3DResourceCB(device, ElementCount, ElementSizeInBytes);
}

TW3DResourceSR* TW3DResourceManager::CreateTexture2D(const TWT::WString& Filename) {
	return TW3DResourceSR::Create2D(device, srv_descriptor_heap, Filename, temp_gcl);
}

TW3DResourceSR* TW3DResourceManager::CreateTextureArray2D(TWT::UInt Width, TWT::UInt Height, TWT::UInt Depth, DXGI_FORMAT Format) {
	TW3DResourceSR* sr = new TW3DResourceSR(device, srv_descriptor_heap, temp_gcl);
	sr->CreateArray2D(Width, Height, Depth, Format);
	return sr;
}

TW3DGraphicsCommandList* TW3DResourceManager::CreateDirectCommandList() {
	return TW3DGraphicsCommandList::CreateDirect(device);
}

TW3DGraphicsCommandList* TW3DResourceManager::CreateBundleCommandList() {
	return TW3DGraphicsCommandList::CreateBundle(device);
}

TW3DGraphicsCommandList* TW3DResourceManager::CreateComputeCommandList() {
	return TW3DGraphicsCommandList::CreateCompute(device);
}

TW3DGraphicsCommandList* TW3DResourceManager::CreateCopyCommandList() {
	return TW3DGraphicsCommandList::CreateCopy(device);
}

TW3DGraphicsCommandList* TW3DResourceManager::GetTemporaryDirectCommandList() {
	temp_direct_cl->Reset();
	temp_direct_cl->BindResources(this);
	return temp_direct_cl;
}

TW3DGraphicsCommandList* TW3DResourceManager::GetTemporaryComputeCommandList() {
	temp_compute_cl->Reset();
	temp_compute_cl->BindResources(this);
	return temp_compute_cl;
}

TW3DGraphicsCommandList* TW3DResourceManager::GetTemporaryCopyCommandList() {
	temp_copy_cl->Reset();
	return temp_copy_cl;
}

TW3DCommandQueue* TW3DResourceManager::GetCommandListCommandQueue(TW3DGraphicsCommandList* CommandList) {
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

TWT::Bool TW3DResourceManager::IsCommandListRunning(TW3DGraphicsCommandList* CommandList) {
	return GetCommandListCommandQueue(CommandList)->IsCommandListRunning(CommandList);
}

void TW3DResourceManager::FlushCommandList(TW3DGraphicsCommandList* CommandList) {
	GetCommandListCommandQueue(CommandList)->FlushCommandList(CommandList);
}

void TW3DResourceManager::FlushCommandLists() {
	copy_command_queue->FlushCommands();
	compute_command_queue->FlushCommands();
	direct_command_queue->FlushCommands();
}

void TW3DResourceManager::ExecuteCommandList(TW3DGraphicsCommandList* CommandList) {
	GetCommandListCommandQueue(CommandList)->ExecuteCommandList(CommandList);
}

void TW3DResourceManager::ExecuteCommandLists(const TWT::Vector<TW3DGraphicsCommandList*>& CommandLists) {
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
