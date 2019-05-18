#pragma once
#include "TW3DResource.h"
#include "TW3DTempGCL.h"

class TW3DResourceVB : public TW3DResource {
public:
	TW3DResourceVB(TW3DDevice* Device, TWT::uint VertexCount, TWT::uint SingleVertexSize, TW3DTempGCL* TempGCL);
	~TW3DResourceVB();

	D3D12_VERTEX_BUFFER_VIEW GetView();
	void UpdateData(const void* Data, TWT::uint Size);
	TWT::uint GetVertexCount();
	TWT::uint GetVertexByteSize();
	TWT::uint GetSizeInBytes();

private:
	TW3DTempGCL* temp_gcl;
	TW3DResource* upload_heap;
	D3D12_VERTEX_BUFFER_VIEW view;
	TWT::uint8* gpu_address = nullptr;
	TWT::uint single_vertex_size, vertex_count;
};