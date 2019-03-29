#include "pch.h"
#include "TW3DObject.h"

TW3D::TW3DObject::TW3DObject(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize) {
	ConstantBuffer = ResourceManager->CreateConstantBuffer(ConstantBufferSize);
}

TW3D::TW3DObject::~TW3DObject() {
	delete ConstantBuffer;
}

void TW3D::TW3DObject::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex) {
	if (transform.Changed) {
		transform.Changed = false;

		TWT::DefaultPerObjectCB cb;
		cb.model = transform.GetModelMatrix();

		ConstantBuffer->Update(&cb, 0);
	}

	CommandList->SetGraphicsRootCBV(ModelCBRootParameterIndex, ConstantBuffer, 0);
}