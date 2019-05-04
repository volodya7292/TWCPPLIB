#include "pch.h"
#include "TW3DResourceCB.h"

TW3D::TW3DResourceCB::TW3DResourceCB(TW3DDevice* Device, TWT::UInt ElementCount, TWT::UInt ElementSize) :
	TW3DResource(Device)
{
	AlignedElementSize = (ElementSize + 255) & ~255;

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(AlignedElementSize * ElementCount),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&Resource);
	Resource->SetName(L"TW3DResourceCB");

	TWU::SuccessAssert(Resource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&GPUAddress)), "TW3DResourceCB::TW3DResourceCB"s);
}

TW3D::TW3DResourceCB::~TW3DResourceCB() {
}

D3D12_GPU_VIRTUAL_ADDRESS TW3D::TW3DResourceCB::GetAddress(TWT::UInt ElementIndex) {
	return GetGPUVirtualAddress() + ElementIndex * AlignedElementSize;
}

void TW3D::TW3DResourceCB::Update(void* Data, TWT::UInt ElementIndex) {
	memcpy(GPUAddress + ElementIndex * AlignedElementSize, Data, AlignedElementSize);
}
