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

void TW3DCube::RecordDraw(TW3DCommandList* CommandList, TWT::uint ModelCBRootParameterIndex) {
	TW3DGeometry::RecordDraw(CommandList, ModelCBRootParameterIndex);

	for (auto& vb : VMInstance.VertexMesh->VertexBuffers) {
		CommandList->SetVertexBuffer(0, vb);
		CommandList->Draw(vb->GetVertexCount());
	}
}
