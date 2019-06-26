#pragma once
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"
#include "TW3DLBVH.h"

class TW3DVertexMesh {
public:
	TW3DVertexMesh(TW3DResourceManager* ResourceManager, const std::vector<TW3DVertexBuffer*>& VertexBuffers);
	~TW3DVertexMesh();

	TWT::uint GetVertexCount();
	TWT::uint GetTriangleCount();
	TW3DLBVH* GetLBVH();

	void UpdateLBVH(TW3DBuffer* GVB, TWT::uint GVBOffset, TW3DCommandList* CommandList = nullptr);

	std::vector<TW3DVertexBuffer*> VertexBuffers;

private:
	bool changed = false;
	TW3DLBVH* lbvh;
};

struct TW3DVertexMeshInstance {
	TW3DTransform Transform;
	TW3DVertexMesh* VertexMesh = nullptr;
	SceneLBVHInstance LBVHInstance = {};
	bool Changed = false;
	rp3d::RigidBody* RigidBody = nullptr;
};