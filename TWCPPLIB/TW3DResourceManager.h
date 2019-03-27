#pragma once
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"
#include "TW3DResourceSV.h"
#include "TW3DTypes.h"

namespace TW3D {
	class TW3DResourceManager {
	public:
		TW3DResourceManager(TW3DDevice* Device);
		~TW3DResourceManager();

		TW3DResourceRTV* CreateRenderTargetView(ID3D12Resource* Buffer);
		TW3DResourceRTV* CreateRenderTargetView(TWT::UInt Width, TWT::UInt Height, DXGI_FORMAT Format, TWT::Vector4f ClearValue);
		TW3DResourceDSV* CreateDepthStencilView(TWT::UInt Width, TWT::UInt Height);
		// By default SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex)
		TW3DResourceVB* CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex));
		TW3DResourceCB* CreateConstantBuffer(TWT::UInt ElementSizeInBytes, TWT::UInt ElementCount = 1);
		TW3DResourceSV* CreateTexture2D(TWT::WString Filename);

		TW3DDevice* GetDevice();
		TW3DDescriptorHeap* GetSVDescriptorHeap();

	private:
		TW3DDevice* device;
		TW3DTempGCL* temp_gcl;
		TW3DDescriptorHeap* rtv_descriptor_heap;
		TW3DDescriptorHeap* sv_descriptor_heap;
		TWT::UInt rtv_descriptor_count = 0;
		TWT::UInt sv_descriptor_count = 0;
	};
}