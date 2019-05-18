#pragma once
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"
#include "TW3DVertexMesh.h"

class TW3DObject {
public:
	TW3DObject(TW3DResourceManager* ResourceManager, TWT::uint ConstantBufferSize);
	virtual ~TW3DObject();

	virtual void Update();
	virtual void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::uint ModelCBRootParameterIndex);

	virtual std::vector<TW3DVertexMeshInstance>& GetVertexMeshInstances() = 0;

protected:
	TW3DResourceCB* ConstantBuffer;

private:
	TW3DTransform DefaultTransform(rp3d::Transform Transform) {
		rp3d::Vector3 initPosition = Transform.getPosition();
		TWT::vec3 position = TWT::vec3(initPosition.x, initPosition.y, initPosition.z);
		rp3d::Quaternion initOrientation = Transform.getOrientation();

		glm::quat quater = glm::quat(initOrientation.x, initOrientation.y, initOrientation.z, initOrientation.w);
		TWT::vec3 rotation = glm::eulerAngles(quater);

		return TW3DTransform(position, rotation);
	}

};