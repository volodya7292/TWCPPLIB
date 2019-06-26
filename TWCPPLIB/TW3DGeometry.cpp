#include "pch.h"
#include "TW3DGeometry.h"

TW3DGeometry::TW3DGeometry(TW3DResourceManager* ResourceManager, TWT::uint ConstantBufferSize) :
	TW3DObject(ResourceManager, ConstantBufferSize)
{
}

void TW3DGeometry::Update() {
	TW3DObject::Update();
}

void TW3DGeometry::RecordDraw(TW3DCommandList* CommandList, TWT::uint ModelCBRootParameterIndex) {
	TW3DObject::RecordDraw(CommandList, ModelCBRootParameterIndex);
}

std::vector<TW3DVertexMeshInstance>& TW3DGeometry::GetVertexMeshInstances() {
	return VMInstances;
}
