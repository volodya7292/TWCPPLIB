#include "pch.h"
#include "TW3DVertexBuffer.h"

TW3D::TW3DVertexBuffer::TW3DVertexBuffer(TW3DDevice* Device, TWT::UInt VertexCount, TWT::UInt SingleVertexSize, TW3DTempGCL* TempGCL) {
	vertex_buffer = new TW3DResourceVB(Device, VertexCount, SingleVertexSize, TempGCL);
	constant_buffer = new TW3DResourceCB(Device, 1, sizeof(TWT::DefaultVertexBufferCB));
}

TW3D::TW3DVertexBuffer::~TW3DVertexBuffer() {
	delete vertex_buffer;
	delete constant_buffer;
}

TW3D::TW3DResourceVB* TW3D::TW3DVertexBuffer::GetVBResource() {
	return vertex_buffer;
}

TW3D::TW3DResourceCB* TW3D::TW3DVertexBuffer::GetCBResource() {
	return constant_buffer;
}

void TW3D::TW3DVertexBuffer::Update(const void* Data, TWT::UInt VertexCount) {
	vertex_buffer->UpdateData(Data, VertexCount);
}

TWT::UInt TW3D::TW3DVertexBuffer::GetVertexOffset() {
	return GVBVertexOffset;
}

void TW3D::TW3DVertexBuffer::SetVertexOffset(TWT::UInt VertexOffset) {
	GVBVertexOffset = VertexOffset;
	TWT::DefaultVertexBufferCB cb;
	cb.vertex_offset.x = VertexOffset;
	constant_buffer->Update(&cb, 0);
}

TWT::UInt TW3D::TW3DVertexBuffer::GetVertexCount() {
	return vertex_buffer->GetVertexCount();
}

TWT::UInt TW3D::TW3DVertexBuffer::GetSizeInBytes() {
	return vertex_buffer->GetSizeInBytes();
}
