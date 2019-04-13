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
		virtual void Resize(TWT::UInt Width, TWT::UInt Height);
		virtual void Record(TW3DScene* Scene, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStencilOutput);
		// Per frame
		virtual void Execute();

	protected:
		TWT::Bool Initialized = false;
		TW3DResourceManager* ResourceManager;
		TW3DSwapChain* SwapChain;
		TWT::UInt Width;
		TWT::UInt Height;
	};
}

