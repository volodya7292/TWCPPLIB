#pragma once
#include "TW3DRenderer.h"

namespace TW3D {
	class TW3DDefaultRenderer : public TW3DRenderer {
	public:
		TW3DDefaultRenderer() = default;
		virtual ~TW3DDefaultRenderer();
		void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
		void Resize(TWT::UInt Width, TWT::UInt Height);
		void Record(TW3DScene* Scene, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStencilOutput);
		void Execute();

	private:
		TW3DGraphicsPipelineState *opaque_raster_ps, *blit_ps;
		TW3DResourceDSV* depthStencil;
		TW3DResourceRTV* offscreen;
		TW3DGraphicsCommandList* command_list;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissor;
	};
}