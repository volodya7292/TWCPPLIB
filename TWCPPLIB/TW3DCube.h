#pragma once
#include "TW3DGeometry.h"

class TW3DCube : public TW3DGeometry {
public:
	TW3DCube(TW3DResourceManager* ResourceManager);
	~TW3DCube() final = default;

	void Update() final;
	void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::uint ModelCBRootParameterIndex)  final;
};