#pragma once
#include "TW3DDevice.h"
#include "TW3DDescriptorHeap.h"
#include "TW3DTempGCL.h"
#include "TW3DTypes.h"

class TW3DResource {
public:
	TW3DResource(TW3DDevice* Device, TW3DTempGCL* TempGCL = nullptr, const CD3DX12_HEAP_PROPERTIES* HeapProperties = nullptr,
		D3D12_RESOURCE_STATES InitialResourceState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, bool OptimizeForUpdating = false,
		D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE, D3D12_CLEAR_VALUE const& ClearValue = { DXGI_FORMAT_UNKNOWN });
	TW3DResource(ID3D12Resource* Resource);
	virtual ~TW3DResource();

	ID3D12Resource* Get();
	D3D12_CLEAR_VALUE GetClearValue();
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

	virtual void Release();

	void Create(const D3D12_RESOURCE_DESC* ResourceDescription = nullptr);
	void Map(TWT::uint SubResourceIndex, D3D12_RANGE* ReadRange, void** Data);
	void Read(void* Out, TWT::uint ByteOffset, TWT::uint ByteCount);

	static TW3DResource* CreateStaging(TW3DDevice* Device, TWT::uint64 Size);

protected:
	TW3DDevice*     device      = nullptr;
	TW3DTempGCL*    temp_gcl    = nullptr;
	TW3DResource*   staging     = nullptr;

	ID3D12Resource* resource = nullptr;
	CD3DX12_HEAP_PROPERTIES heap_properties = {};
	D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE;
	D3D12_RESOURCE_DESC desc = {};
	D3D12_RESOURCE_STATES initial_resource_state;
	D3D12_CLEAR_VALUE clear_value = { DXGI_FORMAT_UNKNOWN };
};

D3D12_RESOURCE_BARRIER TW3DTransitionBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);