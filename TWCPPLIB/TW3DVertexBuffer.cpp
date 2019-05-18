#include "pch.h"
#include "TW3DVertexBuffer.h"

TW3DVertexBuffer::TW3DVertexBuffer(TW3DDevice* Device, TWT::UInt VertexCount, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL) {
	vertex_buffer = new TW3DResourceVB(Device, VertexCount, SingleVertexSize, TempGCL);
}

TW3DVertexBuffer::~TW3DVertexBuffer() {
	delete vertex_buffer;
}

TW3DResourceVB* TW3DVertexBuffer::GetResource() {
	return vertex_buffer;
}

void TW3DVertexBuffer::Update(const void* Data, TWT::UInt VertexCount) {
	vertex_buffer->UpdateData(Data, VertexCount);
}

TWT::UInt TW3DVertexBuffer::GetVertexCount() {
	return vertex_buffer->GetVertexCount();
}

TWT::UInt TW3DVertexBuffer::GetVertexByteSize() {
	return vertex_buffer->GetVertexByteSize();
}

TWT::UInt TW3DVertexBuffer::GetSizeInBytes() {
	return vertex_buffer->GetSizeInBytes();
}
