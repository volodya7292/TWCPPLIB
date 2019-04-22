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
		void Update();
		void Execute(TWT::UInt BackBufferIndex);

	private:
		void CreateGBufferResources();
		void CreateGVBResources();
		void CreateBBCalculatorResources();
		void CreateMortonCalculatorResources();
		void BuildVMAccelerationStructure(TW3DVertexMesh* VertexMesh);

		// Bounding Box Calculator
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *bb_calc_ps;

		TW3DGraphicsCommandList* morton_calc_cl;

		TW3DGraphicsPipelineState *gbuffer_ps, *gvb_ps;
		TW3DComputePipelineState *morton_calc_ps;

		std::unordered_set<TW3DVertexBuffer*> gvb_vertex_buffers;
		std::unordered_set<TW3DVertexMesh*>   gvb_vertex_meshes;
		TW3DResourceUAV* gvb;
		TWT::UInt gvb_vertex_count;
		//*blit_ps;

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		D3D12_RECT scissor = D3D12_RECT();
	};
}