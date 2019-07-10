#include "pch.h"
#include "TW3DConstantBuffer.h"

TW3DConstantBuffer::TW3DConstantBuffer(TWT::String Name, TW3DDevice* Device, TWT::uint ElementCount, TWT::uint ElementSize) :
	TW3DResource(Name, Device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), nullptr, D3D12_RESOURCE_STATE_GENERIC_READ)
{
	AlignedElementSize = (ElementSize + 255) & ~255;

	desc = CD3DX12_RESOURCE_DESC::Buffer(AlignedElementSize * ElementCount),
	TW3DResource::Create();
	Native->SetName(L"TW3DResourceCB");

	Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&GPUAddress));
}

D3D12_GPU_VIRTUAL_ADDRESS TW3DConstantBuffer::GetGPUVirtualAddress(TWT::uint ElementIndex) {
	return Native->GetGPUVirtualAddress() + ElementIndex * AlignedElementSize;
}

void TW3DConstantBuffer::Update(void* Data, TWT::uint ElementIndex) {
	memcpy(GPUAddress + ElementIndex * AlignedElementSize, Data, AlignedElementSize);
}