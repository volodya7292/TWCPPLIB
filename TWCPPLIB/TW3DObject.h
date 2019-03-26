#pragma once
#include "TW3DTypes.h"
#include "TW3DTransform.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DResourceManager.h"

namespace TW3D {
	class TW3DObject {
	public:
		TW3DObject(TW3DResourceManager* ResourceManager, TWT::UInt ConstantBufferSize);
		virtual ~TW3DObject();

		virtual void RecordDraw(TW3DGraphicsCommandList* CommandList, TWT::UInt ModelCBRootParameterIndex);

		TW3DTransform transform;
		
	protected:
		TW3DResourceCB* ConstantBuffer;
	};
}