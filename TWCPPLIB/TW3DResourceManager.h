#pragma once
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"
#include "TW3DResourceSR.h"
#include "TW3DResourceUAV.h"
#include "TW3DCommandQueue.h"
#include "TW3DTypes.h"

namespace TW3D {
	class TW3DResourceManager {
	public:
		TW3DResourceManager(TW3DDevice* Device);
		~TW3DResourceManager();

		TW3DResourceRTV* CreateRenderTargetView(ID3D12Resource* Buffer);
		TW3DResourceRTV* CreateRenderTargetView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Vector4f ClearValue);
		TW3DResourceDSV* CreateDepthStencilView(TWT::UInt Width, TWT::UInt Height);
		TW3DResourceUAV* CreateUnorderedAccessView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format);
		TW3D::TW3DResourceUAV* CreateUnorderedAccessView(TWT::UInt ElementCount, DXGI_FORMAT Format);
		TW3D::TW3DResourceUAV* CreateUnorderedAccessView(TWT::UInt ElementCount, TWT::UInt ElementSizeInBytes);
		// By default SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex)
		TW3DResourceVB* CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex));
		TW3DResourceCB* CreateConstantBuffer(TWT::UInt ElementSizeInBytes, TWT::UInt ElementCount = 1);
		TW3DResourceSR* CreateTexture2D(TWT::WString Filename);
		TW3DResourceSR* CreateTextureArray2D(TWT::UInt Width, TWT::UInt Height, TWT::UInt Depth, DXGI_FORMAT Format);
		TW3DGraphicsCommandList* CreateDirectCommandList();
		TW3DGraphicsCommandList* CreateComputeCommandList();

		void ExecuteCommandList(TW3DGraphicsCommandList* CommandList);
		// Command lists must be of the same type
		void ExecuteCommandLists(const TWT::Vector<TW3DGraphicsCommandList*>& CommandLists);

		void Flush(TW3D::TW3DFence* Fence);

		TW3DDevice* GetDevice();
		TW3DDescriptorHeap* GetSVDescriptorHeap();
		TW3DCommandQueue* GetDirectCommandQueue();

	private:
		TW3DDevice* device;
		TW3DTempGCL* temp_gcl = nullptr;
		TW3DDescriptorHeap* rtv_descriptor_heap = nullptr;
		TW3DDescriptorHeap* dsv_descriptor_heap = nullptr;
		TW3DDescriptorHeap* srv_descriptor_heap = nullptr;
		TW3DCommandQueue* direct_command_queue;
		TW3DCommandQueue* compute_command_queue;
	};
}