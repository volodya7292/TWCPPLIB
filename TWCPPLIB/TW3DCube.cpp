#include "pch.h"
#include "TW3DCube.h"
#include "TW3DPrimitives.h"

TW3D::TW3DCube::TW3DCube(TW3DResourceManager* ResourceManager) :
	TW3DGeometry(ResourceManager, sizeof(TWT::DefaultPerObjectCB))
{
	VertexBuffer = TW3DPrimitives::GetCubeVertexBuffer();
}

void TW3D::TW3DCube::RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex) {
	TW3DGeometry::RecordDraw(CommandList, ModelCBRootParameterIndex);

	CommandList->SetVertexBuffer(0, VertexBuffer);

	CommandList->Draw(36);
}
