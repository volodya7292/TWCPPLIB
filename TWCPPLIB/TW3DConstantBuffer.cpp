#include "pch.h"
#include "TW3DConstantBuffer.h"

TW3DConstantBuffer::TW3DConstantBuffer(TW3DDevice* Device, TWT::uint ElementCount, TWT::uint ElementSize) :
	TW3DResource(Device, nullptr, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_RESOURCE_STATE_GENERIC_READ)
{
	AlignedElementSize = (ElementSize + 255) & ~255;

	desc = CD3DX12_RESOURCE_DESC::Buffer(AlignedElementSize * ElementCount),
	TW3DResource::Create();
	resource->SetName(L"TW3DResourceCB");

	Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&GPUAddress));
}

TW3DConstantBuffer::~TW3DConstantBuffer() {
}

D3D12_GPU_VIRTUAL_ADDRESS TW3DConstantBuffer::GetAddress(TWT::uint ElementIndex) {
	return GetGPUVirtualAddress() + ElementIndex * AlignedElementSize;
}

void TW3DConstantBuffer::Update(void* Data, TWT::uint ElementIndex) {
	memcpy(GPUAddress + ElementIndex * AlignedElementSize, Data, AlignedElementSize);
}
