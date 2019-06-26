#pragma once
#include "TW3DConstantBuffer.h"
#include "TW3DTexture.h"
#include "TW3DBuffer.h"
#include "TW3DRenderTarget.h"
#include "TW3DVertexBuffer.h"
#include "TW3DFramebuffer.h"
#include "TW3DCommandQueue.h"
#include "TW3DTypes.h"

class TW3DResourceManager {
public:
	TW3DResourceManager(TW3DDevice* Device);
	~TW3DResourceManager();

	TW3DFramebuffer* CreateFramebuffer(TWT::uint2 Size);
	TW3DRenderTarget* CreateRenderTarget(ID3D12Resource* Buffer);
	TW3DRenderTarget* CreateRenderTarget(TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	TW3DBuffer* CreateBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes, bool UAV = false);
	TW3DVertexBuffer* CreateVertexBuffer(TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex), bool OptimizeForUpdating = false);
	TW3DConstantBuffer* CreateConstantBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes);
	TW3DTexture* CreateDepthStencilTexture(TWT::uint2 Size);
	TW3DTexture* CreateTexture2D(const TWT::WString& Filename);
	TW3DTexture* CreateTexture2D(TWT::uint2 Size, DXGI_FORMAT Format, bool UAV = false);
	TW3DTexture* CreateTextureArray2D(TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV = false);
	TW3DCommandList* CreateDirectCommandList();
	TW3DCommandList* CreateBundleCommandList();
	TW3DCommandList* CreateComputeCommandList();
	TW3DCommandList* CreateCopyCommandList();
	TW3DCommandList* GetTemporaryDirectCommandList();
	TW3DCommandList* GetTemporaryComputeCommandList();
	TW3DCommandList* GetTemporaryCopyCommandList();
	TW3DCommandQueue* GetCommandListCommandQueue(TW3DCommandList* CommandList);

	void ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	bool IsCommandListRunning(TW3DCommandList* CommandList);
	void FlushCommandList(TW3DCommandList* CommandList);
	void FlushCommandLists();
	void ExecuteCommandList(TW3DCommandList* CommandList);
	// Command lists must be of the same type
	void ExecuteCommandLists(const std::vector<TW3DCommandList*>& CommandLists);

	TW3DDevice* GetDevice();
	TW3DDescriptorHeap* GetSVDescriptorHeap();
	TW3DCommandQueue* GetDirectCommandQueue();

private:
	TW3DDevice* device;
	TW3DTempGCL* temp_gcl;
	TW3DCommandList* temp_direct_cl;
	TW3DCommandList* temp_compute_cl;
	TW3DCommandList* temp_copy_cl;
	TW3DDescriptorHeap* rtv_descriptor_heap;
	TW3DDescriptorHeap* dsv_descriptor_heap;
	TW3DDescriptorHeap* srv_descriptor_heap;
	TW3DDescriptorHeap* uav_descriptor_heap;
	TW3DCommandQueue* direct_command_queue;
	TW3DCommandQueue* compute_command_queue;
	TW3DCommandQueue* copy_command_queue;
};