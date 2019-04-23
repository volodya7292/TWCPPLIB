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
		void CreateGBufferResources();
		void CreateGVBResources();
		void CreateBBCalculatorResources();
		void CreateMortonCalculatorResources();
		void CreateMortonSorterResources();
		void BuildVMAccelerationStructure(TW3DVertexMesh* VertexMesh);


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
		TW3DGraphicsCommandList *morton_calc_cl;

		// Morton codes sorter
		// --------------------------------------------------------------------- 
		TW3DBitonicSort* BitonicSorter;

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		D3D12_RECT scissor = D3D12_RECT();
	};
}