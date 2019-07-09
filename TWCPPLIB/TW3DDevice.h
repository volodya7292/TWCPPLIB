#pragma once
#include "TW3DUtils.h"
#include "TW3DAdapter.h"

class TW3DDevice {
public:
	TW3DDevice(TW3DAdapter* Adapter);
	~TW3DDevice();

	HRESULT GetRemoveReason();
	void GetFeatureData(D3D12_FEATURE Feature, void* FeatureSupportData, TWT::uint FeatureSupportDataSize);

	void CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC* Desc, ID3D12CommandQueue** CommandQueue);
	void CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC* Desc, ID3D12DescriptorHeap** DescriptorHeap);
	void CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator** CommandAllocator);
	void CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator* CommandAllocator, ID3D12PipelineState* InitialState, ID3D12GraphicsCommandList** CommandList);
	void CreateFence(TWT::uint64 InitialValue, D3D12_FENCE_FLAGS Flags, ID3D12Fence** Fence);
	void CreateRootSignature(ID3DBlob* Signature, ID3D12RootSignature** RootSignature);
	void CreateCommandSignature(const D3D12_COMMAND_SIGNATURE_DESC *Desc, ID3D12RootSignature* RootSignature, ID3D12CommandSignature** CommandSignature);
	void CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC *Desc, ID3D12PipelineState** PipelineState);
	void CreateComputePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC* desc, ID3D12PipelineState** PipelineState);
	void CreateCommittedResource(const D3D12_HEAP_PROPERTIES* HeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC* Desc,
		D3D12_RESOURCE_STATES InitialResourceState, ID3D12Resource** Resource, const D3D12_CLEAR_VALUE* OptimizedClearValue = nullptr);

	void CreateRenderTargetView(ID3D12Resource* Resource, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, const D3D12_RENDER_TARGET_VIEW_DESC* Desc = nullptr);
	void CreateDepthStencilView(ID3D12Resource* Resource, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, const D3D12_DEPTH_STENCIL_VIEW_DESC* Desc = nullptr);
	void CreateShaderResourceView(ID3D12Resource* Resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* Desc, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle);
	void CreateUnorderedAccessView(ID3D12Resource* Resource, ID3D12Resource* CounterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle);
	void CreateUnorderedAccessView(ID3D12Resource* Resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc, D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle);

	void MakeResident(ID3D12Pageable* Object);
	void MakeResident(std::vector<ID3D12Pageable*> const& Objects);
	void Evict(ID3D12Pageable* Object);
	void Evict(std::vector<ID3D12Pageable*> const& Objects);

	TWT::uint GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE HeapType);
	TWT::uint64 GetResourceByteSize(const D3D12_RESOURCE_DESC* ResourceDesc, TWT::uint SubResCount);
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT GetSubresourceFootprint(const D3D12_RESOURCE_DESC* ResourceDesc, TWT::uint FirstSubresource, TWT::uint NumSubresources);

private:
	ID3D12Device2* device;
	TW3DAdapter* adapter;
};