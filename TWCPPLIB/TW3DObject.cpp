#include "pch.h"
#include "TW3DObject.h"

TW3DObject::TW3DObject(TW3DResourceManager* ResourceManager) :
	ResourceManager(ResourceManager) {

}

TW3DObject::~TW3DObject() {

}

void TW3DObject::Update() {
	for (auto& vminst : GetVertexMeshInstances()) {
		if (vminst.RigidBody && !vminst.RigidBody->isSleeping()) {
			if (vminst.Transform.Updated)
				vminst.RigidBody->setTransform(vminst.Transform.GetPhysicalTransform());
			else
				vminst.Transform.InitFromPhysicalTransform(vminst.RigidBody->getTransform());
		}
	}
}