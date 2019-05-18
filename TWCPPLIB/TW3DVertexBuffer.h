#pragma once
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"

class TW3DVertexBuffer {
public:
	TW3DVertexBuffer(TW3DDevice* Device, TWT::uint VertexCount, TWT::uint SingleVertexSize, TW3DTempGCL* TempGCL);
	~TW3DVertexBuffer();

	TW3DResourceVB* GetResource();

	void Update(const void* Data, TWT::uint VertexCount);
	TWT::uint GetVertexCount();
	TWT::uint GetVertexByteSize();
	TWT::uint GetSizeInBytes();

private:
	TW3DResourceVB* vertex_buffer;
};