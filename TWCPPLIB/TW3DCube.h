#pragma once
#include "TW3DGeometry.h"

namespace TW3D {
	class TW3DCube : public TW3DGeometry {
	public:
		TW3DCube(TW3DResourceManager* ResourceManager);
		~TW3DCube() = default;

		void Update();
		void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex);
	};
}