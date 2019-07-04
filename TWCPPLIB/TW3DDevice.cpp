#include "pch.h"
#include "TW3DDevice.h"

TW3DDevice::TW3DDevice(TW3DAdapter* adapter) :
	adapter(adapter)
{
	adapter->CreateDevice(&device);
	device->SetName(L"TW3DDevice");
}

TW3DDevice::~TW3DDevice() {
	TWU::DXSafeRelease(device);
}

ID3D12Device2* TW3DDevice::Get() {
	return device;
}

HRESULT TW3DDevice::GetRemoveReason() {
	return device->GetDeviceRemovedReason();
}

void TW3DDevice::GetFeatureData(D3D12_FEATURE Feature, void* FeatureSupportData, TWT::uint FeatureSupportDataSize) {
	TWU::SuccessAssert(device->CheckFeatureSupport(Feature, FeatureSupportData, FeatureSupportDataSize), "TW3DDevice::GetFeatureData"s);
}

void TW3DDevice::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC* desc, ID3D12CommandQueue** commandQueue) {
	TWU::SuccessAssert(device->CreateCommandQueue(desc, IID_PPV_ARGS(commandQueue)), "TW3DDevice::CreateCommandQueue"s);
}

void TW3DDevice::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC* desc, ID3D12DescriptorHeap** descriptorHeap) {
	TWU::SuccessAssert(device->CreateDescriptorHeap(desc, IID_PPV_ARGS(descriptorHeap)), "TW3DDevice::CreateDescriptorHeap"s);
}

void TW3DDevice::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator** commandAllocator) {
	TWU::SuccessAssert(device->CreateCommandAllocator(type, IID_PPV_ARGS(commandAllocator)), "TW3DDevice::CreateCommandAllocator"s);
}

void TW3DDevice::CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* InitialState, ID3D12GraphicsCommandList** commandList) {
	TWU::SuccessAssert(device->CreateCommandList(0, type, commandAllocator, InitialState, IID_PPV_ARGS(commandList)), "TW3DDevice::CreateGraphicsCommandList"s);
}

void TW3DDevice::CreateFence(TWT::uint64 initialValue, D3D12_FENCE_FLAGS flags, ID3D12Fence** fence) {
	TWU::SuccessAssert(device->CreateFence(initialValue, flags, IID_PPV_ARGS(fence)), "TW3DDevice::CreateFence"s);
}

void TW3DDevice::CreateRootSignature(ID3DBlob* signature, ID3D12RootSignature** rootSignature) {
	TWU::SuccessAssert(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature)), "TW3DDevice::CreateRootSignature"s);
}

void TW3DDevice::CreateCommandSignature(const D3D12_COMMAND_SIGNATURE_DESC* Desc, ID3D12RootSignature* RootSignature, ID3D12CommandSignature** CommandSignature) {
	TWU::SuccessAssert(device->CreateCommandSignature(Desc, RootSignature, IID_PPV_ARGS(CommandSignature)), "TW3DDevice::CreateCommandSignature"s);
}

void TW3DDevice::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc, ID3D12PipelineState** pipelineState) {
	TWU::SuccessAssert(device->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(pipelineState)), "TW3DDevice::CreateGraphicsPipelineState"s);
}

void TW3DDevice::CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* desc, ID3D12PipelineState** pipelineState) {
	TWU::SuccessAssert(device->CreateComputePipelineState(desc, IID_PPV_ARGS(pipelineState)), "TW3DDevice::CreateComputePipelineState"s);
}

void TW3DDevice::CreateCommittedResource(const D3D12_HEAP_PROPERTIES* heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
	D3D12_RESOURCE_STATES initialResourceState, ID3D12Resource** resource, const D3D12_CLEAR_VALUE* optimizedClearValue) {

	TWU::SuccessAssert(device->CreateCommittedResource(heapProperties, heapFlags, desc, initialResourceState, optimizedClearValue, IID_PPV_ARGS(resource)), "TW3DDevice::CreateCommittedResource"s);
}

void TW3DDevice::CreateRenderTargetView(ID3D12Resource* rtv, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, const D3D12_RENDER_TARGET_VIEW_DESC* desc) {
	device->CreateRenderTargetView(rtv, desc, cpuDescHandle);
}

void TW3DDevice::CreateDepthStencilView(ID3D12Resource* dsv, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc) {
	device->CreateDepthStencilView(dsv, desc, cpuDescHandle);
}

void TW3DDevice::CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor) {
	device->CreateShaderResourceView(resource, desc, destDescriptor);
}

void TW3DDevice::CreateUnorderedAccessView(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest_descriptor) {
	device->CreateUnorderedAccessView(resource, nullptr, desc, dest_descriptor);
}

void TW3DDevice::MakeResident(ID3D12Pageable* Object) {
	TWU::SuccessAssert(device->MakeResident(1, &Object), "TW3DDevice::MakeResident Single"s);
}

void TW3DDevice::MakeResident(std::vector<ID3D12Pageable*> const& Objects) {
	TWU::SuccessAssert(device->MakeResident(Objects.size(), Objects.data()), "TW3DDevice::MakeResident : "s + Objects.size());
}

void TW3DDevice::Evict(ID3D12Pageable* Object) {
	TWU::SuccessAssert(device->Evict(1, &Object), "TW3DDevice::Evict Single"s);
}

void TW3DDevice::Evict(std::vector<ID3D12Pageable*> const& Objects) {
	TWU::SuccessAssert(device->Evict(Objects.size(), Objects.data()), "TW3DDevice::Evict : "s + Objects.size());
}

TWT::uint64 TW3DDevice::GetCopyableFootprints(const D3D12_RESOURCE_DESC* resourceDesc, TWT::uint subResCount) {
	TWT::uint64 totalBytes;
	device->GetCopyableFootprints(resourceDesc, 0, subResCount, 0, nullptr, nullptr, nullptr, &totalBytes);
	return totalBytes;
}

TWT::uint TW3DDevice::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType) {
	return device->GetDescriptorHandleIncrementSize(heapType);
}
