#pragma once
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"
#include "TW3DVertexMesh.h"

class TW3DObject {
public:
	TW3DObject(TW3DResourceManager* ResourceManager, TWT::uint ConstantBufferSize);
	virtual ~TW3DObject();

	virtual void Update();
	virtual void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::uint ModelCBRootParameterIndex);

	virtual std::vector<TW3DVertexMeshInstance>& GetVertexMeshInstances() = 0;

protected:
	TW3DConstantBuffer* ConstantBuffer;
	TWT::DefaultModelCB cb_data;
};