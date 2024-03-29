#pragma once
#include "TW3DResource.h"
#include "TW3DTypes.h"

enum TW3DRenderTargetType {
	TW3D_RENDER_TARGET_UNKNOWN,
	TW3D_RENDER_TARGET_2D,
	TW3D_RENDER_TARGET_CUBE
};


class TW3DRenderTarget : public TW3DResource {
public:
	TW3DRenderTarget(TWT::String Name, TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TW3DDescriptorHeap* SRVDescriptorHeap);
	TW3DRenderTarget(TWT::String Name, TW3DDevice* Device, TW3DDescriptorHeap* RTVDescriptorHeap, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	~TW3DRenderTarget() final;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPURTVHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSRVHandle();

	void Create(ID3D12Resource* Buffer);
	void Create(TWT::uint2 Size);
	void CreateCube(TWT::uint Size);
	void Resize(TWT::uint2 Size);

	TWT::uint2 GetSize();

private:
	TW3DDescriptorHeap*	rtv_descriptor_heap;
	TW3DDescriptorHeap* srv_descriptor_heap = nullptr;

	int RTVIndex = -1, SRVIndex = -1;
	TW3DRenderTargetType type;
};