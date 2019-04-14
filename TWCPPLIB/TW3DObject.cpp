#include "pch.h"
#include "TW3DObject.h"

TW3D::TW3DObject::TW3DObject(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize) {
	ConstantBuffer = ResourceManager->CreateConstantBuffer(ConstantBufferSize);
}

TW3D::TW3DObject::~TW3DObject() {
	delete ConstantBuffer;
}

void TW3D::TW3DObject::Update() {
	if (Transform.Changed) {
		Transform.Changed = false;

		TWT::DefaultPerObjectCB cb;
		cb.model = Transform.GetModelMatrix();

		ConstantBuffer->Update(&cb, 0);
	}
}

void TW3D::TW3DObject::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex) {
	CommandList->SetRootCBV(ModelCBRootParameterIndex, ConstantBuffer, 0);
}