#pragma once
#include "TW3DBitonicSorter.h"
#include "TW3DVertexMesh.h"

namespace TW3D {
	class TW3DLBVHBuilder {
	public:
		TW3DLBVHBuilder(TW3DResourceManager* ResourceManager);
		~TW3DLBVHBuilder();
		
		void SetCommandList(TW3DGraphicsCommandList* CommandList);

		void Build(TW3DResourceUAV* GVB, TW3DVertexMesh* VertexMesh);
		void Build(TW3DResourceUAV* GNB, TW3DVertexMesh* VertexMesh);

	private:
		TW3DGraphicsCommandList* command_list;

		TW3DBitonicSorter* bitonic_sorter;
		TW3DComputePipelineState* bounding_box_calc_ps;
		TW3DComputePipelineState* morton_calc_ps;
		TW3DComputePipelineState* setup_lbvh_nodes_ps;
		TW3DComputePipelineState* build_lbvh_splits_ps;
		TW3DComputePipelineState* update_lbvh_node_boundaries;
	};
}