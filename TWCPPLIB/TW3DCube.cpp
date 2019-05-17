#include "pch.h"
#include "TW3DCube.h"
#include "TW3DPrimitives.h"

TW3D::TW3DCube::TW3DCube(TW3DResourceManager* ResourceManager) :
	TW3DGeometry(ResourceManager, sizeof(TWT::DefaultVertexBufferCB))
{
	VMInstances[0].VertexMesh = TW3DPrimitives::GetCubeVertexMesh();
}

void TW3D::TW3DCube::Update() {
	TW3DGeometry::Update();
}

void TW3D::TW3DCube::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex) {
	TW3DGeometry::RecordDraw(CommandList, ModelCBRootParameterIndex);

	CommandList->SetVertexBuffer(0, VMInstances[0].VertexMesh->VertexBuffers[0]->GetResource());

	CommandList->Draw(VMInstances[0].VertexMesh->VertexBuffers[0]->GetVertexCount());
}
