#include "pch.h"
#include "TW3DDevice.h"

TW3D::TW3DDevice::TW3DDevice(TW3D::TW3DAdapter* adapter) :
	adapter(adapter)
{
	adapter->CreateDevice(&device);
	device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &SupportedFeatures, sizeof(SupportedFeatures));
}

TW3D::TW3DDevice::~TW3DDevice() {
	TWU::DXSafeRelease(device);
}

D3D12_FEATURE_DATA_D3D12_OPTIONS TW3D::TW3DDevice::GetSupportedFeatures() {
	return SupportedFeatures;
}

void TW3D::TW3DDevice::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC* desc, ID3D12CommandQueue** commandQueue) {
	TWU::SuccessAssert(device->CreateCommandQueue(desc, IID_PPV_ARGS(commandQueue)));
}

void TW3D::TW3DDevice::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC* desc, ID3D12DescriptorHeap** descriptorHeap) {
	TWU::SuccessAssert(device->CreateDescriptorHeap(desc, IID_PPV_ARGS(descriptorHeap)));
}

void TW3D::TW3DDevice::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator** commandAllocator) {
	TWU::SuccessAssert(device->CreateCommandAllocator(type, IID_PPV_ARGS(commandAllocator)));
}

void TW3D::TW3DDevice::CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList** commandList) {
	TWU::SuccessAssert(device->CreateCommandList(0, type, commandAllocator, nullptr, IID_PPV_ARGS(commandList)));
}

void TW3D::TW3DDevice::CreateFence(TWT::UInt64 initialValue, D3D12_FENCE_FLAGS flags, ID3D12Fence1** fence) {
	TWU::SuccessAssert(device->CreateFence(initialValue, flags, IID_PPV_ARGS(fence)));
}
#include <comdef.h>
void TW3D::TW3DDevice::CreateRootSignature(ID3DBlob* signature, ID3D12RootSignature** rootSignature) {
	TWU::SuccessAssert(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature)));
}

void TW3D::TW3DDevice::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc, ID3D12PipelineState** pipelineState) {
	TWU::SuccessAssert(device->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(pipelineState)));
}

void TW3D::TW3DDevice::CreateCommittedResource(const D3D12_HEAP_PROPERTIES* heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
	D3D12_RESOURCE_STATES initialResourceState, ID3D12Resource** resource, const D3D12_CLEAR_VALUE* optimizedClearValue) {

	TWU::SuccessAssert(device->CreateCommittedResource(heapProperties, heapFlags, desc, initialResourceState, optimizedClearValue, IID_PPV_ARGS(resource)));
}

void TW3D::TW3DDevice::CreateRenderTargetView(ID3D12Resource* rtv, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, const D3D12_RENDER_TARGET_VIEW_DESC* desc) {
	device->CreateRenderTargetView(rtv, desc, cpuDescHandle);
}

void TW3D::TW3DDevice::CreateDepthStencilView(ID3D12Resource* dsv, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc) {
	device->CreateDepthStencilView(dsv, desc, cpuDescHandle);
}

void TW3D::TW3DDevice::CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor) {
	device->CreateShaderResourceView(resource, desc, destDescriptor);
}

TWT::UInt64 TW3D::TW3DDevice::GetCopyableFootprints(const D3D12_RESOURCE_DESC* resourceDesc, TWT::UInt subResCount) {
	TWT::UInt64 totalBytes;
	device->GetCopyableFootprints(resourceDesc, 0, subResCount, 0, nullptr, nullptr, nullptr, &totalBytes);
	return totalBytes;
}

TWT::UInt TW3D::TW3DDevice::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType) {
	return device->GetDescriptorHandleIncrementSize(heapType);
}
