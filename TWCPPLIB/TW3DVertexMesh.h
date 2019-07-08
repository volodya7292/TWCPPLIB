#pragma once
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"

class TW3DVertexMesh {
public:
	TW3DVertexMesh(TW3DResourceManager* ResourceManager, const std::vector<TW3DVertexBuffer*>& VertexBuffers);
	~TW3DVertexMesh();

	TWT::uint GetVertexCount();
	TWT::uint GetTriangleCount();

	void UpdateLBVH(TW3DBuffer* GVB, TWT::uint GVBOffset, TW3DCommandList* CommandList = nullptr);

	std::vector<TW3DVertexBuffer*> VertexBuffers;

private:
	bool changed = false;
};

struct TW3DVertexMeshInstance {
	TW3DTransform Transform;
	TW3DVertexMesh* VertexMesh = nullptr;
	bool Changed = false;
	rp3d::RigidBody* RigidBody = nullptr;
};