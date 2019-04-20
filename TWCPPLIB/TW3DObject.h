#pragma once
#include "TW3DTypes.h"
#include "TW3DResourceManager.h"
#include "TW3DVertexMesh.h"

namespace TW3D {
	class TW3DObject {
	public:
		TW3DObject(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize);
		virtual ~TW3DObject();

		virtual void Update();
		virtual void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex);
		
		TW3DVertexMeshInstance VMInstance;

	protected:
		TW3DResourceCB* ConstantBuffer;
	};
}