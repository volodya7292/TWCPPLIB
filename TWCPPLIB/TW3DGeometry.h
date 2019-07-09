#pragma once
#include "TW3DObject.h"

class TW3DGeometry : public TW3DObject {
private:
	std::vector<TW3DVertexMeshInstance> VMInstances = std::vector<TW3DVertexMeshInstance>(1);

public:
	TW3DGeometry(TW3DResourceManager* ResourceManager);
	~TW3DGeometry() override = default;

	void Update() override;

	std::vector<TW3DVertexMeshInstance>& GetVertexMeshInstances() override;

	TW3DVertexMeshInstance& VMInstance = VMInstances[0];
};
