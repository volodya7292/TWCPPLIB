#include "pch.h"
#include "TW3DGeometry.h"

TW3D::TW3DGeometry::TW3DGeometry(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize) :
	TW3DObject(ResourceManager, ConstantBufferSize)
{
	VMInstances.resize(1);
}

void TW3D::TW3DGeometry::Update() {
	TW3DObject::Update();
}

void TW3D::TW3DGeometry::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex) {
	TW3DObject::RecordDraw(CommandList, ModelCBRootParameterIndex);
}
