#include "pch.h"
#include "TW3DObject.h"

TW3DObject::TW3DObject(TW3DResourceManager* ResourceManager, TWT::uint ConstantBufferSize) {
	ConstantBuffer = ResourceManager->CreateConstantBuffer(1, ConstantBufferSize);
}

TW3DObject::~TW3DObject() {
	delete ConstantBuffer;
}

void TW3DObject::Update() {
	for (auto& vminst : GetVertexMeshInstances()) {
		vminst.RigidBody->setIsAllowedToSleep(true);

		if (!vminst.RigidBody->isSleeping()) {
			if (vminst.Transform.Updated)
				vminst.RigidBody->setTransform(vminst.Transform.GetPhysicalTransform());
			else
				vminst.Transform.InitFromPhysicalTransform(vminst.RigidBody->getTransform());

			vminst.Changed = true;
			vminst.Transform.Updated = false;

			TWT::DefaultModelCB cb;
			cb.model = vminst.Transform.GetModelMatrix();

			ConstantBuffer->Update(&cb, 0);
		}
	}
}

void TW3DObject::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::uint ModelCBRootParameterIndex) {
	CommandList->BindConstantBuffer(ModelCBRootParameterIndex, ConstantBuffer);
}