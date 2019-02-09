#include "pch.h"
#include "TW3DResourceVB.h"

TW3D::TW3DResourceVB::TW3DResourceVB(TW3DDevice* Device, TWT::UInt64 Size, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), Size(Size), TempGCL(TempGCL)
{
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		D3D12_RESOURCE_STATE_COPY_DEST,
		&Resource);

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&UploadHeap);

	View.BufferLocation = Resource->GetGPUVirtualAddress();
	View.SizeInBytes = Size;
	View.StrideInBytes = SingleVertexSize;
}

TW3D::TW3DResourceVB::~TW3DResourceVB() {
	TWU::DXSafeRelease(UploadHeap);
}

D3D12_VERTEX_BUFFER_VIEW TW3D::TW3DResourceVB::GetView() {
	return View;
}

void TW3D::TW3DResourceVB::UpdateData(BYTE* Data, TWT::UInt64 Size) {
	D3D12_SUBRESOURCE_DATA vertexData ={};
	vertexData.pData = Data;
	vertexData.RowPitch = Size;
	vertexData.SlicePitch = Size;

	TempGCL->Reset();

	TempGCL->UpdateSubresources(Resource, UploadHeap, &vertexData);
	TempGCL->ResourceBarrier(Resource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	TempGCL->Execute();

}
