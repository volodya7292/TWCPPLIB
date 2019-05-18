#pragma once
#include "TW3DResourceCB.h"
#include "TW3DResourceSR.h"
#include "TW3DResourceUAV.h"
#include "TW3DVertexBuffer.h"
#include "TW3DCommandQueue.h"
#include "TW3DTypes.h"


class TW3DResourceManager {
public:
	TW3DResourceManager(TW3DDevice* Device);
	~TW3DResourceManager();

	TW3DResourceRTV* CreateRenderTargetView(ID3D12Resource* Buffer);
	TW3DResourceRTV* CreateRenderTargetView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Vector4f ClearValue);
	TW3DResourceDSV* CreateDepthStencilView(TWT::UInt Width, TWT::UInt Height);
	TW3DResourceUAV* CreateUnorderedAccessView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format);
	TW3DResourceUAV* CreateUnorderedAccessView(TWT::UInt ElementCount, TWT::UInt ElementSizeInBytes);
	TW3DVertexBuffer* CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex));
	TW3DResourceCB* CreateConstantBuffer(TWT::UInt ElementCount, TWT::UInt ElementSizeInBytes);
	TW3DResourceSR* CreateTexture2D(const TWT::WString& Filename);
	TW3DResourceSR* CreateTextureArray2D(TWT::UInt Width, TWT::UInt Height, TWT::UInt Depth, DXGI_FORMAT Format);
	TW3DGraphicsCommandList* CreateDirectCommandList();
	TW3DGraphicsCommandList* CreateBundleCommandList();
	TW3DGraphicsCommandList* CreateComputeCommandList();
	TW3DGraphicsCommandList* CreateCopyCommandList();
	TW3DGraphicsCommandList* GetTemporaryDirectCommandList();
	TW3DGraphicsCommandList* GetTemporaryComputeCommandList();
	TW3DGraphicsCommandList* GetTemporaryCopyCommandList();
	TW3DCommandQueue* GetCommandListCommandQueue(TW3DGraphicsCommandList* CommandList);

	void ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	TWT::Bool IsCommandListRunning(TW3DGraphicsCommandList* CommandList);
	void FlushCommandList(TW3DGraphicsCommandList* CommandList);
	void FlushCommandLists();
	void ExecuteCommandList(TW3DGraphicsCommandList* CommandList);
	// Command lists must be of the same type
	void ExecuteCommandLists(const TWT::Vector<TW3DGraphicsCommandList*>& CommandLists);

	TW3DDevice* GetDevice();
	TW3DDescriptorHeap* GetSVDescriptorHeap();
	TW3DCommandQueue* GetDirectCommandQueue();

private:
	TW3DDevice* device;
	TW3DTempGCL* temp_gcl;
	TW3DGraphicsCommandList* temp_direct_cl;
	TW3DGraphicsCommandList* temp_compute_cl;
	TW3DGraphicsCommandList* temp_copy_cl;
	TW3DDescriptorHeap* rtv_descriptor_heap;
	TW3DDescriptorHeap* dsv_descriptor_heap;
	TW3DDescriptorHeap* srv_descriptor_heap;
	TW3DCommandQueue* direct_command_queue;
	TW3DCommandQueue* compute_command_queue;
	TW3DCommandQueue* copy_command_queue;
};