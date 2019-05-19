#pragma once
#include "TW3DConstantBuffer.h"
#include "TW3DTexture.h"
#include "TW3DBuffer.h"
#include "TW3DResourceDSV.h"
#include "TW3DRenderTarget.h"
#include "TW3DVertexBuffer.h"
#include "TW3DCommandQueue.h"
#include "TW3DTypes.h"

class TW3DResourceManager {
public:
	TW3DResourceManager(TW3DDevice* Device);
	~TW3DResourceManager();

	TW3DRenderTarget* CreateRenderTargetView(ID3D12Resource* Buffer);
	TW3DRenderTarget* CreateRenderTargetView(TWT::uint Width, TWT::uint Height, DXGI_FORMAT Format, TWT::vec4 ClearValue);
	TW3DResourceDSV* CreateDepthStencilView(TWT::uint Width, TWT::uint Height);
	TW3DBuffer* CreateBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes, bool UAV = false);
	TW3DVertexBuffer* CreateVertexBuffer(TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex));
	TW3DConstantBuffer* CreateConstantBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes);
	TW3DTexture* CreateTexture2D(const TWT::WString& Filename);
	TW3DTexture* CreateTexture2D(TWT::uint Width, TWT::uint Height, DXGI_FORMAT Format, bool UAV = false);
	TW3DTexture* CreateTextureArray2D(TWT::uint Width, TWT::uint Height, TWT::uint Depth, DXGI_FORMAT Format, bool UAV = false);
	TW3DGraphicsCommandList* CreateDirectCommandList();
	TW3DGraphicsCommandList* CreateBundleCommandList();
	TW3DGraphicsCommandList* CreateComputeCommandList();
	TW3DGraphicsCommandList* CreateCopyCommandList();
	TW3DGraphicsCommandList* GetTemporaryDirectCommandList();
	TW3DGraphicsCommandList* GetTemporaryComputeCommandList();
	TW3DGraphicsCommandList* GetTemporaryCopyCommandList();
	TW3DCommandQueue* GetCommandListCommandQueue(TW3DGraphicsCommandList* CommandList);

	void ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	bool IsCommandListRunning(TW3DGraphicsCommandList* CommandList);
	void FlushCommandList(TW3DGraphicsCommandList* CommandList);
	void FlushCommandLists();
	void ExecuteCommandList(TW3DGraphicsCommandList* CommandList);
	// Command lists must be of the same type
	void ExecuteCommandLists(const std::vector<TW3DGraphicsCommandList*>& CommandLists);

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