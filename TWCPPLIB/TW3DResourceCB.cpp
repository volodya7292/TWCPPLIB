#include "pch.h"
#include "TW3DResourceCB.h"

TW3D::TW3DResourceCB::TW3DResourceCB(TW3DDevice* Device) :
	TW3DResource(Device)
{
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&Resource);

	Resource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&GPUAddress));
}

TW3D::TW3DResourceCB::~TW3DResourceCB() {
}

void TW3D::TW3DResourceCB::Update(void* Data, TWT::UInt Size, TWT::UInt Align) {
	memcpy(GPUAddress + Align, Data, Size);
}
