#pragma once
#include "TW3DResourceVB.h"
#include "TW3DObject.h"

namespace TW3D {
	class TW3DGeometry : public TW3DObject {
	public:
		TW3DGeometry(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize);
		virtual ~TW3DGeometry() = default;

		virtual void Update();
		virtual void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex);

	protected:
		TW3DResourceVB* VertexBuffer;
	};
}