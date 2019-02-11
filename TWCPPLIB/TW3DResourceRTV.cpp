#include "pch.h"
#include "TW3DResourceRTV.h"

TW3D::TW3DResourceRTV::TW3DResourceRTV(TW3DDevice* Device, TW3DDescriptorHeap* DescriptorHeap) :
	TW3DResource(Device), DescriptorHeap(DescriptorHeap)
{
}

TW3D::TW3DResourceRTV::~TW3DResourceRTV() {
	
}

D3D12_CPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceRTV::GetHandle() {
	return DescriptorHeap->GetHandle(Offset, Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

void TW3D::TW3DResourceRTV::Create(ID3D12Resource* Buffer, TWT::Int Offset) {
	this->Offset = Offset;
	Resource = Buffer;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetHandle();
	Device->CreateRenderTargetView(Resource, rtvHandle);
}

void TW3D::TW3DResourceRTV::Release() {
	TW3DResource::Release();
}
