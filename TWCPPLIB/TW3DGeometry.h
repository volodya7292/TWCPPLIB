#pragma once
#include "TW3DObject.h"

class TW3DGeometry : public TW3DObject {
public:
	TW3DGeometry(TW3DResourceManager* ResourceManager, TWT::uint ConstantBufferSize);
	~TW3DGeometry() override = default;

	void Update() override;
	void RecordDraw(TW3DCommandList* CommandList, TWT::uint ModelCBRootParameterIndex) override;

	std::vector<TW3DVertexMeshInstance>& GetVertexMeshInstances() override;

private:
	std::vector<TW3DVertexMeshInstance> VMInstances = std::vector<TW3DVertexMeshInstance>(1);

public:
	TW3DVertexMeshInstance& VMInstance = VMInstances[0];
};
