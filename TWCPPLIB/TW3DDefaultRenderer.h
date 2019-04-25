#pragma once
#include "TW3DRenderer.h"
#include "TW3DBitonicSort.h"

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
		void CreateBlitResources();
		void CreateGBufferResources();
		void CreateGVBResources();
		void CreateBBCalculatorResources();
		void CreateMortonCalculatorResources();
		void CreateMortonSorterResources();
		void CreateLBVHLeavesSetupResources();
		void CreateLBVHSplitsBuildResources();
		void CreateLBVHNodeUpdateBoundariesResources();

		void BuildVMAccelerationStructure(TW3DVertexMesh* VertexMesh);
		void BlitOutput(TW3DGraphicsCommandList* cl, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* Depth);

		TW3DResourceUAV* rt_output;

		// Mesh AS Build commandl list
		// --------------------------------------------------------------------- 
		TW3DGraphicsCommandList *mesh_as_cl;


		// GBuffer render
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *gbuffer_ps;

		// Global vertex buffer builder
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *gvb_ps;
		std::unordered_set<TW3DVertexBuffer*> gvb_vertex_buffers;
		std::unordered_set<TW3DVertexMesh*>   gvb_vertex_meshes;
		TW3DResourceUAV* gvb;
		TWT::UInt gvb_vertex_count;

		// Mesh bounding box calculator
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState* bb_calc_ps;

		// Morton codes calculator
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *morton_calc_ps;

		// Morton codes sorter
		// --------------------------------------------------------------------- 
		TW3DBitonicSort* BitonicSorter;

		// Setup LBVH nodes
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *setup_lbvh_nodes_ps;

		// Build LBVH splits
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *build_lbvh_splits_ps;

		// Update LBVH node boundaries
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *update_lbvh_node_boundaries;

		// Ray tracing compute
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *rt_ps;

		// Blit output
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *blit_ps;

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		D3D12_RECT scissor = D3D12_RECT();
	};
}