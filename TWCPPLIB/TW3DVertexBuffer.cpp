#include "pch.h"
#include "TW3DVertexBuffer.h"

TW3DVertexBuffer::TW3DVertexBuffer(TW3DDevice* Device, TWT::uint VertexCount, TWT::uint SingleVertexSize, TW3DTempGCL* TempGCL) {
	vertex_buffer = new TW3DResourceVB(Device, VertexCount, SingleVertexSize, TempGCL);
}

TW3DVertexBuffer::~TW3DVertexBuffer() {
	delete vertex_buffer;
}

TW3DResourceVB* TW3DVertexBuffer::GetResource() {
	return vertex_buffer;
}

void TW3DVertexBuffer::Update(const void* Data, TWT::uint VertexCount) {
	vertex_buffer->UpdateData(Data, VertexCount);
}

TWT::uint TW3DVertexBuffer::GetVertexCount() {
	return vertex_buffer->GetVertexCount();
}

TWT::uint TW3DVertexBuffer::GetVertexByteSize() {
	return vertex_buffer->GetVertexByteSize();
}

TWT::uint TW3DVertexBuffer::GetSizeInBytes() {
	return vertex_buffer->GetSizeInBytes();
}
