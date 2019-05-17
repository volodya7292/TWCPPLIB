#include "pch.h"
#include "TW3DObject.h"

TW3D::TW3DObject::TW3DObject(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize) {
	ConstantBuffer = ResourceManager->CreateConstantBuffer(1, ConstantBufferSize);
}

TW3D::TW3DObject::~TW3DObject() {
	delete ConstantBuffer;
}

void TW3D::TW3DObject::Update() {
	for (auto& vminst : VMInstances) {

		vminst.Transform = DefaultTransform(vminst.RigidBody->getTransform());
		vminst.Transform.Changed = true;
		if (vminst.Transform.Changed) {
			vminst.Changed = true;
			vminst.Transform.Changed = false;

			TWT::DefaultModelCB cb;
			cb.model = vminst.Transform.GetModelMatrix();

			ConstantBuffer->Update(&cb, 0);
		}
	}
}

void TW3D::TW3DObject::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex) {
	CommandList->SetRootCBV(ModelCBRootParameterIndex, ConstantBuffer, 0);
}