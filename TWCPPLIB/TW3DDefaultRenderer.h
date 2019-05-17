#pragma once
#include "TW3DRenderer.h"

namespace TW3D {
	class TW3DDefaultRenderer : public TW3DRenderer {
	public:
		TW3DDefaultRenderer() = default;
		virtual ~TW3DDefaultRenderer();
		void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
		void Resize(TWT::UInt Width, TWT::UInt Height);
		void Record(TWT::UInt BackBufferIndex, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStesncilOutput);
		void RecordBeforeExecution();
		void Update(TWT::Float DeltaTime);
		void Execute(TWT::UInt BackBufferIndex);

	private:
		void CreateBlitResources();
		void CreateGBufferResources();
		void CreateRTResources();

		void BlitOutput(TW3DGraphicsCommandList* cl, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* Depth);

		TW3DResourceUAV* rt_output = nullptr;

		TW3DGraphicsCommandList* rt_cl = nullptr;   // Ray tracing CL

		// GBuffer render
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *gbuffer_ps = nullptr;

		// Ray tracing compute
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *rt_ps = nullptr;

		// Blit output
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *blit_ps = nullptr;

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		D3D12_RECT scissor = D3D12_RECT();
	};
}