#include "pch.h"
#include "TW3DCube.h"
#include "TW3DPrimitives.h"

TW3DCube::TW3DCube(TW3DResourceManager* ResourceManager) :
	TW3DGeometry(ResourceManager, sizeof(TWT::DefaultModelCB))
{
	VMInstance.VertexMesh = TW3DPrimitives::GetCubeVertexMesh();
}

void TW3DCube::Update() {
	TW3DGeometry::Update();
}

void TW3DCube::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::uint ModelCBRootParameterIndex) {
	TW3DGeometry::RecordDraw(CommandList, ModelCBRootParameterIndex);

	CommandList->SetVertexBuffer(0, VMInstance.VertexMesh->VertexBuffers[0]);

	CommandList->Draw(VMInstance.VertexMesh->VertexBuffers[0]->GetVertexCount());
}
