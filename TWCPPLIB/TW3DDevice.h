#pragma once
#include "TW3DUtils.h"
#include "TW3DAdapter.h"

class TW3DDevice {
public:
	TW3DDevice(TW3DAdapter* adapter);
	~TW3DDevice();

	ID3D12Device2* Get();

	HRESULT GetRemoveReason();
	void GetFeatureData(D3D12_FEATURE Feature, void* FeatureSupportData, TWT::uint FeatureSupportDataSize);

	void CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC* desc, ID3D12CommandQueue** commandQueue);
	void CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC* desc, ID3D12DescriptorHeap** descriptorHeap);
	void CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator** commandAllocator);
	void CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* commandAllocator, ID3D12GraphicsCommandList** commandList);
	void CreateFence(TWT::uint64 initialValue, D3D12_FENCE_FLAGS flags, ID3D12Fence** fence);
	void CreateRootSignature(ID3DBlob* signature, ID3D12RootSignature** rootSignature);
	void CreateCommandSignature(const D3D12_COMMAND_SIGNATURE_DESC *Desc, ID3D12RootSignature* RootSignature, ID3D12CommandSignature** CommandSignature);
	void CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC *desc, ID3D12PipelineState** pipelineState);
	void CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* desc, ID3D12PipelineState** pipelineState);
	void CreateCommittedResource(const D3D12_HEAP_PROPERTIES* heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
		D3D12_RESOURCE_STATES initialResourceState, ID3D12Resource** resource, const D3D12_CLEAR_VALUE* optimizedClearValue = nullptr);

	void CreateRenderTargetView(ID3D12Resource* rtv, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, const D3D12_RENDER_TARGET_VIEW_DESC* desc = nullptr);
	void CreateDepthStencilView(ID3D12Resource* dsv, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc = nullptr);
	void CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE destDescriptor);
	void CreateUnorderedAccessView(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE dest_descriptor);

	TWT::uint GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType);
	TWT::uint64 GetCopyableFootprints(const D3D12_RESOURCE_DESC* resourceDesc, TWT::uint subResCount);

private:
	TW3DAdapter* adapter;
	ID3D12Device2* device;
};