#include "pch.h"
#include "TW3DResourceVB.h"

TW3D::TW3DResourceVB::TW3DResourceVB(TW3DDevice* Device, TWT::UInt64 Size, TW3DTempGCL* TempGCL) :
	TW3DResource(Device), Size(Size), TempGCL(TempGCL)
{
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		D3D12_RESOURCE_STATE_COPY_DEST,
		&Resource);
}

TW3D::TW3DResourceVB::~TW3DResourceVB() {

}

void TW3D::TW3DResourceVB::UpdateData(BYTE* Data, TWT::UInt64 Size) {
	TWT::Float64 t = TWU::GetTime();

	ID3D12Resource* res;
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&res);

	D3D12_SUBRESOURCE_DATA vertexData ={};
	vertexData.pData = Data; // pointer to our vertex array
	vertexData.RowPitch = Size; // size of all our triangle vertex data
	vertexData.SlicePitch = Size;

	TempGCL->Reset();
	TempGCL->UpdateSubresources(Resource, res, &vertexData);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	TempGCL->ResourceBarrier(this, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	TempGCL->Execute();

	TWU::DXSafeRelease(res);

	TWT::Float64 t2 = TWU::GetTime();
	TWU::CPrintln(t2 - t);
}
