#pragma once
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"
#include "TW3DVertexMesh.h"

namespace TW3D {
	class TW3DObject {
	public:
		TW3DObject(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize);
		virtual ~TW3DObject();

		virtual void Update();
		virtual void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex);
		
		std::vector<TW3DVertexMeshInstance> VMInstances;

	protected:
		TW3DResourceCB* ConstantBuffer;

	private:
		TW3D::TW3DTransform DefaultTransform(rp3d::Transform Transform) {
			rp3d::Vector3 initPosition = Transform.getPosition();
			TWT::Vector3f position = TWT::Vector3f(initPosition.x, initPosition.y, initPosition.z);
			rp3d::Quaternion initOrientation = Transform.getOrientation();

			glm::quat quater = glm::quat(initOrientation.x, initOrientation.y, initOrientation.z, initOrientation.w);
			TWT::Vector3f rotation = glm::eulerAngles(quater);

			return TW3DTransform(position, rotation);
		}

	};
}