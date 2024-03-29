#pragma once
#include "TW3DDevice.h"
#include "TW3DDescriptorHeap.h"
#include "TW3DTempGCL.h"
#include "TW3DTypes.h"

class TW3DResource {
public:
	TW3DResource(TWT::String Name, TW3DDevice* Device, CD3DX12_HEAP_PROPERTIES const& HeapProperties, TW3DTempGCL* TempGCL = nullptr,
		D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, bool UpdateOptimized = false,
		D3D12_HEAP_FLAGS HeapFlags = D3D12_HEAP_FLAG_NONE, D3D12_CLEAR_VALUE const& ClearValue = { DXGI_FORMAT_UNKNOWN });
	TW3DResource(TWT::String Name, ID3D12Resource* Resource);
	virtual ~TW3DResource();

	D3D12_RESOURCE_DESC GetDescription();
	D3D12_CLEAR_VALUE GetClearValue();
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();
	TWT::String const& GetName();

	virtual void Release();

	void Create();
	void Create(D3D12_RESOURCE_DESC const& ResourceDescription);
	void Map(TWT::uint SubResourceIndex, const D3D12_RANGE* ReadRange, void** Data);
	void Unmap(TWT::uint SubResourceIndex, const D3D12_RANGE* WrittenRange);
	void Read(void* Out, TWT::uint ByteOffset, TWT::uint ByteCount) const;

	D3D12_RESOURCE_STATES InitialState  = D3D12_RESOURCE_STATE_GENERIC_READ;
	D3D12_RESOURCE_FLAGS  ResourceFlags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* Native = nullptr;

protected:
	void AllocateStaging();
	void DeallocateStaging();

	TW3DDevice*     device      = nullptr;
	TW3DTempGCL*    temp_gcl    = nullptr;
	TW3DResource*   staging     = nullptr;
	TWT::uint8*     staging_addr;

	const bool update_optimized;

	CD3DX12_HEAP_PROPERTIES heap_properties = {};
	D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE;
	D3D12_RESOURCE_DESC desc = {};
	D3D12_CLEAR_VALUE clear_value = { DXGI_FORMAT_UNKNOWN };
	TWT::String name;
};

D3D12_RESOURCE_BARRIER TW3DTransitionBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);