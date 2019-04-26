#pragma once
#include "TW3DRenderer.h"
#include "TW3DLBVHBuilder.h"

namespace TW3D {
	class TW3DDefaultRenderer : public TW3DRenderer {
	public:
		TW3DDefaultRenderer() = default;
		virtual ~TW3DDefaultRenderer();
		void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
		void Resize(TWT::UInt Width, TWT::UInt Height);
		void Record(TWT::UInt BackBufferIndex, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStesncilOutput);
		void RecordBeforeExecution();
		void Update();
		void Execute(TWT::UInt BackBufferIndex);

	private:
		void CreateBlitResources();
		void CreateGBufferResources();
		void CreateGVBResources();
		void CreateGLBVHNodeBufferResources();
		void CreateRTResources();

		void BlitOutput(TW3DGraphicsCommandList* cl, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* Depth);

		TW3DResourceUAV* rt_output;

		TW3DGraphicsCommandList* lbvh_cl; // CL for building (global) LBVH node buffer
		TW3DGraphicsCommandList* rt_cl;   // Ray tracing CL

		// GBuffer render
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *gbuffer_ps;

		// Global vertex buffer builder
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *gvb_ps;
		TWT::Vector<TW3DVertexBuffer*> gvb_vertex_buffers;
		TWT::Vector<TW3DVertexMesh*>   gvb_vertex_meshes;
		TW3DResourceUAV* gvb;
		TWT::UInt gvb_vertex_count;

		// LBVHs global buffer
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *glbvh_nodes_ps;
		TW3DResourceUAV* g_lbvh_nodes;
		TWT::UInt g_lbvh_node_count;


		// Ray tracing compute
		// --------------------------------------------------------------------- 
		TW3DComputePipelineState *rt_ps;
		TW3DLBVHBuilder* lbvh_builder;

		// Blit output
		// --------------------------------------------------------------------- 
		TW3DGraphicsPipelineState *blit_ps;

		D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
		D3D12_RECT scissor = D3D12_RECT();
	};
}