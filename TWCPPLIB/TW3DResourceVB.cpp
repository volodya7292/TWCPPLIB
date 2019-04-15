#include "pch.h"
#include "TW3DResourceVB.h"

TW3D::TW3DResourceVB::TW3DResourceVB(TW3DDevice* Device, TWT::UInt Size, TWT::UInt SingleVertexSize) :
	TW3DResource(Device), Size(Size), SingleVertexSize(SingleVertexSize) {

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Size),
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&Resource);
	Resource->SetName(L"TW3DResourceVB");

	TWU::SuccessAssert(Resource->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&gpu_address)));

	View.BufferLocation = Resource->GetGPUVirtualAddress();
	View.SizeInBytes    = Size;
	View.StrideInBytes  = SingleVertexSize;
}

TW3D::TW3DResourceVB::~TW3DResourceVB() {	
}

D3D12_VERTEX_BUFFER_VIEW TW3D::TW3DResourceVB::GetView() {
	return View;
}

void TW3D::TW3DResourceVB::UpdateData(const void* Data, TWT::UInt Size) {
	memcpy(gpu_address, Data, Size);
	VertexCount = Size / SingleVertexSize;
}

TWT::UInt TW3D::TW3DResourceVB::GetVertexCount() {
	return VertexCount;
}

TWT::UInt TW3D::TW3DResourceVB::GetSizeInBytes() {
	return Size;
}