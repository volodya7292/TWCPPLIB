#include "pch.h"
#include "TW3DResourceUAV.h"

TW3D::TW3DResourceUAV::TW3DResourceUAV(TW3DDevice* Device, TW3DDescriptorHeap* SRVDescriptorHeap, TWT::UInt ElementSizeInBytes) :
	TW3DResource(Device), SRVDescriptorHeap(SRVDescriptorHeap), element_size(ElementSizeInBytes) {
	SRVIndex = SRVDescriptorHeap->Allocate(); // For SRV
	SRVDescriptorHeap->Allocate(); // For UAV

	desc = CD3DX12_RESOURCE_DESC::Buffer(0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.StructureByteStride = ElementSizeInBytes;
	srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.StructureByteStride = ElementSizeInBytes;
	uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
}

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
	return SRVDescriptorHeap->GetGPUHandle(SRVIndex + 1);
}

void TW3D::TW3DResourceUAV::CreateBuffer(TWT::UInt ElementCount) {
	desc.Width = ElementCount * element_size;

	srv_desc.Buffer.NumElements = ElementCount;
	uav_desc.Buffer.NumElements = ElementCount;

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		&Resource);

	Device->CreateShaderResourceView(Resource, &srv_desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex));
	Device->CreateUnorderedAccessView(Resource, &uav_desc, SRVDescriptorHeap->GetCPUHandle(SRVIndex + 1));
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
	Device->CreateUnorderedAccessView(Resource, nullptr, SRVDescriptorHeap->GetCPUHandle(SRVIndex + 1));
}
