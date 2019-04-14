#pragma once
#include "TW3DRenderer.h"

namespace TW3D {
	class TW3DDefaultRenderer : public TW3DRenderer {
	public:
		TW3DDefaultRenderer() = default;
		virtual ~TW3DDefaultRenderer();
		void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
		void Resize(TWT::UInt Width, TWT::UInt Height);
		void Record(const TWT::Vector<TW3DResourceRTV*>& ColorOutputs, TW3DResourceDSV* DepthStencilOutput);
		void Update();
		void Execute(TWT::UInt BackBufferIndex);

	private:
		TW3DGraphicsPipelineState *opaque_raster_ps = 0, *blit_ps = 0;
		TWT::Vector<TW3D::TW3DGraphicsCommandList*> command_lists;
		TWT::UInt current_record_index = 0;

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		D3D12_RECT scissor = D3D12_RECT();
	};
}