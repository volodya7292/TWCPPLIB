#pragma once
#include "TW3DGraphicsCommandList.h"
#include "TW3DScene.h"

namespace TW3D {
	class TW3DRenderingMethod {
	public:
		TW3DRenderingMethod() = default;
		virtual ~TW3DRenderingMethod() = default;
		virtual void UpdateCommandList(TW3DGraphicsCommandList* CommandList, TW3DScene* Scene);
	};
}

