#include "pch.h"
#include "TW3DDevice.h"

TW3DDevice::TW3DDevice(TW3DAdapter* Adapter) :
	adapter(Adapter)
{
	adapter->CreateDevice(&device);
	device->SetName(L"TW3DDevice");
}

TW3DDevice::~TW3DDevice() {
	TWU::DXSafeRelease(device);
}

HRESULT TW3DDevice::GetRemoveReason() {
	return device->GetDeviceRemovedReason();
}

void TW3DDevice::GetFeatureData(D3D12_FEATURE Feature, void* FeatureSupportData, TWT::uint FeatureSupportDataSize) {
	TWU::SuccessAssert(device->CheckFeatureSupport(Feature, FeatureSupportData, FeatureSupportDataSize), "TW3DDevice::GetFeatureData"s);
}

void TW3DDevice::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC* Desc, ID3D12CommandQueue** CommandQueue) {
	TWU::SuccessAssert(device->CreateCommandQueue(Desc, IID_PPV_ARGS(CommandQueue)), "TW3DDevice::CreateCommandQueue"s);
}

void TW3DDevice::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC* Desc, ID3D12DescriptorHeap** DescriptorHeap) {
	TWU::SuccessAssert(device->CreateDescriptorHeap(Desc, IID_PPV_ARGS(DescriptorHeap)), "TW3DDevice::CreateDescriptorHeap"s);
}

void TW3DDevice::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator** CommandAllocator) {
	TWU::SuccessAssert(device->CreateCommandAllocator(Type, IID_PPV_ARGS(CommandAllocator)), "TW3DDevice::CreateCommandAllocator"s);
}

void TW3DDevice::CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator* CommandAllocator, ID3D12PipelineState* InitialState, ID3D12GraphicsCommandList** CommandList) {
	TWU::SuccessAssert(device->CreateCommandList(0, Type, CommandAllocator, InitialState, IID_PPV_ARGS(CommandList)), "TW3DDevice::CreateGraphicsCommandList"s);
}

void TW3DDevice::CreateFence(TWT::uint64 InitialValue, D3D12_FENCE_FLAGS Flags, ID3D12Fence** Fence) {
	TWU::SuccessAssert(device->CreateFence(InitialValue, Flags, IID_PPV_ARGS(Fence)), "TW3DDevice::CreateFence"s);
}

void TW3DDevice::CreateRootSignature(ID3DBlob* Signature, ID3D12RootSignature** RootSignature) {
	TWU::SuccessAssert(device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(RootSignature)), "TW3DDevice::CreateRootSignature"s);
}

void TW3DDevice::CreateCommandSignature(const D3D12_COMMAND_SIGNATURE_DESC* Desc, ID3D12RootSignature* RootSignature, ID3D12CommandSignature** CommandSignature) {
	TWU::SuccessAssert(device->CreateCommandSignature(Desc, RootSignature, IID_PPV_ARGS(CommandSignature)), "TW3DDevice::CreateCommandSignature"s);
}

void TW3DDevice::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* Desc, ID3D12PipelineState** PipelineState) {
	TWU::SuccessAssert(device->CreateGraphicsPipelineState(Desc, IID_PPV_ARGS(PipelineState)), "TW3DDevice::CreateGraphicsPipelineState"s);
}

void TW3DDevice::CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* Desc, ID3D12PipelineState** PipelineState) {
	TWU::SuccessAssert(device->CreateComputePipelineState(Desc, IID_PPV_ARGS(PipelineState)), "TW3DDevice::CreateComputePipelineState"s);
}

void TW3DDevice::CreateCommittedResource(const D3D12_HEAP_PROPERTIES* HeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC* Desc,
	D3D12_RESOURCE_STATES InitialResourceState, ID3D12Resource** Resource, const D3D12_CLEAR_VALUE* OptimizedClearValue) {

	TWU::SuccessAssert(device->CreateCommittedResource(HeapProperties, HeapFlags, Desc, InitialResourceState, OptimizedClearValue, IID_PPV_ARGS(Resource)), "TW3DDevice::CreateCommittedResource"s);
}

void TW3DDevice::CreateRenderTargetView(ID3D12Resource* Resource, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, const D3D12_RENDER_TARGET_VIEW_DESC* Desc) {
	device->CreateRenderTargetView(Resource, Desc, CpuHandle);
}

void TW3DDevice::CreateDepthStencilView(ID3D12Resource* Resource, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC* Desc) {
	device->CreateDepthStencilView(Resource, Desc, CpuHandle);
}

void TW3DDevice::CreateShaderResourceView(ID3D12Resource* Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* Desc, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle) {
	device->CreateShaderResourceView(Resource, Desc, CpuHandle);
}

void TW3DDevice::CreateUnorderedAccessView(ID3D12Resource* Resource, ID3D12Resource* CounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle) {
	device->CreateUnorderedAccessView(Resource, CounterResource, Desc, CpuHandle);
}

void TW3DDevice::CreateUnorderedAccessView(ID3D12Resource* Resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle) {
	device->CreateUnorderedAccessView(Resource, nullptr, Desc, CpuHandle);
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

TWT::uint64 TW3DDevice::GetResourceByteSize(const D3D12_RESOURCE_DESC* ResourceDesc, TWT::uint SubResourceCount) {
	TWT::uint64 total_bytes;
	device->GetCopyableFootprints(ResourceDesc, 0, SubResourceCount, 0, nullptr, nullptr, nullptr, &total_bytes);
	return total_bytes;
}

TWT::uint TW3DDevice::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE HeapType) {
	return device->GetDescriptorHandleIncrementSize(HeapType);
}
