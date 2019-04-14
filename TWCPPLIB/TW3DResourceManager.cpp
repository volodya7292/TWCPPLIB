#include "pch.h"
#include "TW3DResourceManager.h"

TW3D::TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device)
{
	temp_gcl = new TW3DTempGCL(device);
	rtv_descriptor_heap = TW3DDescriptorHeap::CreateForRTV(device, 1024);
	dsv_descriptor_heap = TW3DDescriptorHeap::CreateForDSV(device, 1024);
	srv_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 1024);
	direct_command_queue = TW3DCommandQueue::CreateDirect(device);
	compute_command_queue = TW3DCommandQueue::CreateCompute(device);
}

TW3D::TW3DResourceManager::~TW3DResourceManager() {
	delete temp_gcl;
	delete rtv_descriptor_heap;
	delete dsv_descriptor_heap;
	delete srv_descriptor_heap;
	delete direct_command_queue;
	delete compute_command_queue;
}

TW3D::TW3DResourceRTV* TW3D::TW3DResourceManager::CreateRenderTargetView(ID3D12Resource* Buffer) {
	TW3DResourceRTV* rtv = new TW3D::TW3DResourceRTV(device, rtv_descriptor_heap);
	rtv->Create(Buffer);
	return rtv;
}

TW3D::TW3DResourceRTV* TW3D::TW3DResourceManager::CreateRenderTargetView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Vector4f ClearValue) {
	TW3DResourceRTV* rtv = new TW3DResourceRTV(device, rtv_descriptor_heap, srv_descriptor_heap, Format, ClearValue);
	rtv->Create(Width, Height);
	return rtv;
}

TW3D::TW3DResourceDSV* TW3D::TW3DResourceManager::CreateDepthStencilView(TWT::UInt Width, TWT::UInt Height) {
	TW3DResourceDSV* dsv = new TW3DResourceDSV(device, dsv_descriptor_heap);
	dsv->Create(Width, Height);
	return dsv;
}

TW3D::TW3DResourceUAV* TW3D::TW3DResourceManager::CreateUnorderedAccessView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format) {
	TW3DResourceUAV* uav = new TW3DResourceUAV(device, srv_descriptor_heap, Format);
	uav->CreateTexture2D(Width, Height);
	return uav;
}

TW3D::TW3DResourceUAV* TW3D::TW3DResourceManager::CreateUnorderedAccessView(TWT::UInt ElementCount, DXGI_FORMAT Format = DXGI_FORMAT_R8_UINT) {
	TW3DResourceUAV* uav = new TW3DResourceUAV(device, srv_descriptor_heap, Format);
	uav->CreateBuffer(ElementCount);
	return uav;
}

TW3D::TW3DResourceUAV* TW3D::TW3DResourceManager::CreateUnorderedAccessView(TWT::UInt ElementCount, TWT::UInt ElementSizeInBytes) {
	TW3DResourceUAV* uav = new TW3DResourceUAV(device, srv_descriptor_heap, ElementSizeInBytes);
	uav->CreateBuffer(ElementCount);
	return uav;
}

TW3D::TW3DResourceVB* TW3D::TW3DResourceManager::CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes) {
	return new TW3DResourceVB(device, VertexCount * SingleVertexSizeInBytes, SingleVertexSizeInBytes, temp_gcl);
}

TW3D::TW3DResourceCB* TW3D::TW3DResourceManager::CreateConstantBuffer(TWT::UInt ElementSizeInBytes, TWT::UInt ElementCount) {
	return new TW3DResourceCB(device, ElementSizeInBytes, ElementCount);
}

TW3D::TW3DResourceSR* TW3D::TW3DResourceManager::CreateTexture2D(TWT::WString Filename) {
	return TW3DResourceSR::Create2D(device, srv_descriptor_heap, Filename, temp_gcl);
}

TW3D::TW3DResourceSR* TW3D::TW3DResourceManager::CreateTextureArray2D(TWT::UInt Width, TWT::UInt Height, TWT::UInt Depth, DXGI_FORMAT Format) {
	TW3DResourceSR* sr = new TW3DResourceSR(device, srv_descriptor_heap, temp_gcl);
	sr->CreateArray2D(Width, Height, Depth, Format);
	return sr;
}

TW3D::TW3DGraphicsCommandList* TW3D::TW3DResourceManager::CreateDirectCommandList() {
	return TW3DGraphicsCommandList::CreateDirect(device);
}

TW3D::TW3DGraphicsCommandList* TW3D::TW3DResourceManager::CreateBundleCommandList() {
	return TW3DGraphicsCommandList::CreateBundle(device);
}

TW3D::TW3DGraphicsCommandList* TW3D::TW3DResourceManager::CreateComputeCommandList() {
	return TW3DGraphicsCommandList::CreateCompute(device);
}

void TW3D::TW3DResourceManager::ExecuteCommandList(TW3DGraphicsCommandList* CommandList) {
	if (CommandList->Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
		direct_command_queue->ExecuteCommandList(CommandList);
	else
		compute_command_queue->ExecuteCommandList(CommandList);
}

void TW3D::TW3DResourceManager::ExecuteCommandLists(const TWT::Vector<TW3DGraphicsCommandList*>& CommandLists) {
	if (CommandLists[0]->Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
		direct_command_queue->ExecuteCommandLists(CommandLists);
	else
		compute_command_queue->ExecuteCommandLists(CommandLists);
}

void TW3D::TW3DResourceManager::Flush(TW3D::TW3DFence* Fence) {
	Fence->Flush(direct_command_queue);
	Fence->Flush(compute_command_queue);
}

TW3D::TW3DDevice* TW3D::TW3DResourceManager::GetDevice() {
	return device;
}

TW3D::TW3DDescriptorHeap* TW3D::TW3DResourceManager::GetSVDescriptorHeap() {
	return srv_descriptor_heap;
}

TW3D::TW3DCommandQueue* TW3D::TW3DResourceManager::GetDirectCommandQueue() {
	return direct_command_queue;
}
