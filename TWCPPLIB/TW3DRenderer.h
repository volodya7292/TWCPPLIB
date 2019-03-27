#pragma once
#include "TW3DSwapChain.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DScene.h"

namespace TW3D {
	class TW3DRenderer {
	public:
		TW3DRenderer() = default;
		virtual ~TW3DRenderer() = default;
		virtual void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
		virtual void UpdateCommandList(TW3DGraphicsCommandList* CommandList, TW3DScene* Scene);
	};
}

