#pragma once
#include "TW3DResource.h"

class TW3DVertexBuffer : public TW3DResource {
public:
	TW3DVertexBuffer(TW3DDevice* Device, TW3DTempGCL* TempGCL, bool OptimizeForUpdating, TWT::uint VertexCount, TWT::uint SingleVertexSize);
	~TW3DVertexBuffer() final = default;

	D3D12_VERTEX_BUFFER_VIEW GetView();
	void Update(const void* Data, TWT::uint VertexCount);
	TWT::uint GetVertexCount();
	TWT::uint GetVertexByteSize();
	TWT::uint GetSizeInBytes();

private:
	D3D12_VERTEX_BUFFER_VIEW view;
	TWT::uint8* gpu_address = nullptr;
	TWT::uint single_vertex_size, vertex_count;
};