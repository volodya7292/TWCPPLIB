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

		vminst.Transform = DefaultTransform(vminst.RigidBody->getTransform());
		vminst.Transform.Changed = !vminst.RigidBody->isSleeping();

		if (vminst.Transform.Changed) {

			TWT::mat4 ind = glm::inverse(vminst.Transform.GetModelMatrix());
			TWT::vec3 n = TWT::Normalize(TWT::vec3(1, 2, 3));
			TWT::vec4 v = ind * TWT::vec4(n, 0);


			vminst.Changed = true;
			vminst.Transform.Changed = false;

			TWT::DefaultModelCB cb;
			cb.model = vminst.Transform.GetModelMatrix();

			ConstantBuffer->Update(&cb, 0);
		}
	}
}

void TW3DObject::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::uint ModelCBRootParameterIndex) {
	CommandList->BindConstantBuffer(ModelCBRootParameterIndex, ConstantBuffer);
}