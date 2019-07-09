#pragma once
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"
#include "TW3DTransform.h"

struct TW3DVertexMeshInstance {
	TW3DTransform Transform;
	TW3DVertexBuffer* VertexBuffer = nullptr;
	rp3d::RigidBody* RigidBody = nullptr;
	bool CreateRigidBody = false;
};

class TW3DObject {
public:
	TW3DObject(TW3DResourceManager* ResourceManager);
	virtual ~TW3DObject();

	virtual void Update();

	virtual std::vector<TW3DVertexMeshInstance>& GetVertexMeshInstances() = 0;

protected:
	TW3DResourceManager* ResourceManager;
};