#include "pch.h"
#include "TW3DResourceUAV.h"


TW3D::TW3DResourceUAV::TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, DXGI_FORMAT Format) :
	TW3DResource(Device), SRVDescriptorHeap(SRVDescriptorHeap)
{
	SRVIndex = SRVDescriptorHeap->Allocate(); // For SRV
	SRVDescriptorHeap->Allocate(); // For UAV

	desc = CD3DX12_RESOURCE_DESC::Tex2D(Format, 0, 0, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
}


TW3D::TW3DResourceUAV::~TW3DResourceUAV() {

}

D3D12_GPU_DESCRIPTOR_HANDLE TW3D::TW3DResourceUAV::GetGPUHandle() {
	return SRVDescriptorHeap->GetGPUHandle(SRVIndex);
}

void TW3D::TW3DResourceUAV::CreateTexture2D(TWT::UInt Width, TWT::UInt Height) {
	desc.Width = Width;
	desc.Height = Height;

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		&Resource);

	Device->CreateShaderResourceView(Resource, nullptr, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
	Device->CreateUnorderedAccessView(Resource, SRVDescriptorHeap->GetCPUHandle(SRVIndex + 1));
}
