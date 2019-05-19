#include "pch.h"
#include "TW3DShaders.h"
#include "CalculateTriangleMeshBoundingBox.c.h"
#include "CalculateMortonCodesForTriangles.c.h"
#include "SetupLBVHNodesFromTriangles.c.h"
#include "BuildLBVHSplits.c.h"
#include "UpdateLBVHNodeBounds.c.h"
#include "CalculateLBVHsBoundingBox.c.h"
#include "CalculateMortonCodesForLBVHs.c.h"
#include "SetupLBVHNodesFromLBVHs.c.h"
#include "BuildLBVHSplitsFromLBVHs.c.h"
#include "UpdateLBVHNodeBoundsForLBVHs.c.h"

std::vector<TW3DComputePipelineState*> compute_shaders;
std::vector<TW3DGraphicsPipelineState*> graphics_shaders;

void TW3DShaders::Initialize(TW3DResourceManager* ResourceManager) {
	compute_shaders.resize(ComputeShaderCount);
	graphics_shaders.resize(GraphicsShaderCount);

	TW3DDevice* device = ResourceManager->GetDevice();

	TW3DRootSignature* rs0 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4)
		},
		false, false, false, false
		);

	//TW3DRootSignature* rs0 = new TW3DRootSignature(false, false, false, false);
	//rs0->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	//rs0->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0); // Bounding box UAV
	//rs0->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4); // Input data constants
	//rs0->Create(device);

	TW3DComputePipelineState* triangle_mesh_bounding_box_calc_ps = new TW3DComputePipelineState(rs0);
	triangle_mesh_bounding_box_calc_ps->SetShader(TW3DCompiledShader(CalculateTriangleMeshBoundingBox_ByteCode));
	triangle_mesh_bounding_box_calc_ps->Create(device);
	compute_shaders[CalculateTriangleMeshBoundingBox] = triangle_mesh_bounding_box_calc_ps;


	TW3DRootSignature* rs1 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 1),
			TW3DRPBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 2)
		},
		false, false, false, false
	);

	//TW3DRootSignature* rs1 = new TW3DRootSignature(false, false, false, false);
	//rs1->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	//rs1->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Bounding box Buffer SRV
	//rs1->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer UAV
	////rs1->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer UAV
	//rs1->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	//rs1->Create(device);

	TW3DComputePipelineState* triangle_morton_calc_ps = new TW3DComputePipelineState(rs1);
	triangle_morton_calc_ps->SetShader(TW3DCompiledShader(CalculateMortonCodesForTriangles_ByteCode));
	triangle_morton_calc_ps->Create(device);
	compute_shaders[CalculateMortonCodesForTriangles] = triangle_morton_calc_ps;


	TW3DRootSignature* rs2 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 1),
			TW3DRPBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 3)
		},
		false, false, false, false
	);

	//TW3DRootSignature* rs2 = new TW3DRootSignature(false, false, false, false);
	//rs2->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	//rs2->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer SRV
	//rs2->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	////rs2->SetParameterUAVBuffer(4, D3D12_SHADER_VISIBILITY_ALL, 1); // LBVH node lock buffer UAV to clear
	//rs2->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 3); // Input data constants
	//rs2->Create(device);

	TW3DComputePipelineState* triangles_setup_lbvh_nodes_ps = new TW3DComputePipelineState(rs2);
	triangles_setup_lbvh_nodes_ps->SetShader(TW3DCompiledShader(SetupLBVHNodesFromTriangles_ByteCode));
	triangles_setup_lbvh_nodes_ps->Create(device);
	compute_shaders[SetupLBVHNodesFromTriangles] = triangles_setup_lbvh_nodes_ps;


	TW3DRootSignature* rs3 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 2)
		},
		false, false, false, false
	);

	//TW3DRootSignature* rs3 = new TW3DRootSignature(false, false, false, false);
	//rs3->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer SRV
	//rs3->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs3->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	//rs3->Create(device);

	TW3DComputePipelineState* build_lbvh_splits_ps = new TW3DComputePipelineState(rs3);
	build_lbvh_splits_ps->SetShader(TW3DCompiledShader(BuildLBVHSplits_ByteCode));
	build_lbvh_splits_ps->Create(device);
	compute_shaders[BuildLBVHSplits] = build_lbvh_splits_ps;


	TW3DRootSignature* rs4 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 2)
		},
		false, false, false, false
	);
	//TW3DRootSignature* rs4 = new TW3DRootSignature(false, false, false, false);
	//rs4->SetParameterUAVBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs4->SetParameterConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	//rs4->Create(device);

	TW3DComputePipelineState* update_lbvh_node_bounds = new TW3DComputePipelineState(rs4);
	update_lbvh_node_bounds->SetShader(TW3DCompiledShader(UpdateLBVHNodeBounds_ByteCode));
	update_lbvh_node_bounds->Create(device);
	compute_shaders[UpdateLBVHNodeBounds] = update_lbvh_node_bounds;


	TW3DRootSignature* rs6 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 1),
			TW3DRPBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 3)
		},
		false, false, false, false
	);
	//TW3DRootSignature* rs6 = new TW3DRootSignature(false, false, false, false);
	//rs6->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Node Buffer SRV
	//rs6->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Global Node Buffer offsets SRV
	//rs6->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // Bounding box UAV
	//rs6->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 3); // Input data constants
	//rs6->Create(device);

	TW3DComputePipelineState* lbvhs_bounding_box_calc_ps = new TW3DComputePipelineState(rs6);
	lbvhs_bounding_box_calc_ps->SetShader(TW3DCompiledShader(CalculateLBVHsBoundingBox_ByteCode));
	lbvhs_bounding_box_calc_ps->Create(device);
	compute_shaders[CalculateLBVHsBoundingBox] = lbvhs_bounding_box_calc_ps;


	TW3DRootSignature* rs7 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 1),
			TW3DRPBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 2),
			TW3DRPBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 1)
		},
		false, false, false, false
	);
	//TW3DRootSignature* rs7 = new TW3DRootSignature(false, false, false, false);
	//rs7->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Node Buffer SRV
	//rs7->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Global Node Buffer offsets SRV
	//rs7->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Bounding box Buffer SRV
	//rs7->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer UAV
	////rs7->SetParameterUAVBuffer(4, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer UAV
	//rs7->SetParameterConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	//rs7->Create(device);

	TW3DComputePipelineState* lbvhs_morton_calc_ps = new TW3DComputePipelineState(rs7);
	lbvhs_morton_calc_ps->SetShader(TW3DCompiledShader(CalculateMortonCodesForLBVHs_ByteCode));
	lbvhs_morton_calc_ps->Create(device);
	compute_shaders[CalculateMortonCodesForLBVHs] = lbvhs_morton_calc_ps;


	TW3DRootSignature* rs8 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 1),
			TW3DRPBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 2),
			TW3DRPBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 3)
		},
		false, false, false, false
	);
	//TW3DRootSignature* rs8 = new TW3DRootSignature(false, false, false, false);
	//rs8->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Node Buffer SRV
	//rs8->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Global Node Buffer offsets SRV
	//rs8->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Morton code data buffer SRV
	//rs8->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs8->SetParameterConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 3); // Input data constants
	//rs8->Create(device);

	TW3DComputePipelineState* lbvhs_setup_lbvh_nodes_ps = new TW3DComputePipelineState(rs8);
	lbvhs_setup_lbvh_nodes_ps->SetShader(TW3DCompiledShader(SetupLBVHNodesFromLBVHs_ByteCode));
	lbvhs_setup_lbvh_nodes_ps->Create(device);
	compute_shaders[SetupLBVHNodesFromLBVHs] = lbvhs_setup_lbvh_nodes_ps;


	TW3DRootSignature* rs9 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 2)
		},
		false, false, false, false
	);
	//TW3DRootSignature* rs9 = new TW3DRootSignature(false, false, false, false);
	//rs9->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer SRV
	//rs9->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs9->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	//rs9->Create(device);

	TW3DComputePipelineState* lbvhs_build_lbvh_splits_ps = new TW3DComputePipelineState(rs9);
	lbvhs_build_lbvh_splits_ps->SetShader(TW3DCompiledShader(BuildLBVHSplitsFromLBVHs_ByteCode));
	lbvhs_build_lbvh_splits_ps->Create(device);
	compute_shaders[BuildLBVHSplitsFromLBVHs] = lbvhs_build_lbvh_splits_ps;


	TW3DRootSignature* rs10 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 2)
		},
		false, false, false, false
	);
	//TW3DRootSignature* rs10 = new TW3DRootSignature(false, false, false, false);
	//rs10->SetParameterUAVBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs10->SetParameterConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	//rs10->Create(device);

	TW3DComputePipelineState* lbvhs_update_lbvh_node_bounds = new TW3DComputePipelineState(rs10);
	lbvhs_update_lbvh_node_bounds->SetShader(TW3DCompiledShader(UpdateLBVHNodeBoundsForLBVHs_ByteCode));
	lbvhs_update_lbvh_node_bounds->Create(device);
	compute_shaders[UpdateLBVHNodeBoundsForLBVHs] = lbvhs_update_lbvh_node_bounds;


	// -----------------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------



}

void TW3DShaders::Release() {
	for (TW3DComputePipelineState* ps : compute_shaders)
		delete ps;
	for (TW3DGraphicsPipelineState* ps : graphics_shaders)
		delete ps;
}

TW3DComputePipelineState* TW3DShaders::GetComputeShader(TWT::uint Id) {
	return compute_shaders[Id];
}

TW3DGraphicsPipelineState* TW3DShaders::GetGraphicsShader(TWT::uint Id) {
	return graphics_shaders[Id];
}
