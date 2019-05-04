#include "pch.h"
#include "TW3DShaders.h"
#include "CalculateMeshBoundingBox.c.h"
#include "CalculateMortonCodes.c.h"
#include "SetupLBVHNodes.c.h"
#include "BuildLBVHSplits.c.h"
#include "UpdateLBVHNodeBounds.c.h"
#include "BuildGlobalLBVHNodeBuffer.c.h"
#include "CalculateLBVHsBoundingBox.c.h"
#include "CalculateMortonCodesForLBVHs.c.h"
#include "SetupLBVHNodesFromLBVHs.c.h"
#include "BuildLBVHSplitsFromLBVHs.c.h"
#include "UpdateLBVHNodeBoundsForLBVHs.c.h"

TWT::Vector<TW3D::TW3DComputePipelineState*> compute_shaders;
TWT::Vector<TW3D::TW3DGraphicsPipelineState*> graphics_shaders;

void TW3DShaders::Initialize(TW3D::TW3DResourceManager* ResourceManager) {
	compute_shaders.resize(ComputeShaderCount);
	graphics_shaders.resize(GraphicsShaderCount);

	TW3D::TW3DDevice* device = ResourceManager->GetDevice();

	TW3D::TW3DRootSignature* rs0 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs0->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs0->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0); // Bounding box UAV
	rs0->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4); // Input data constants
	rs0->Create(device);

	TW3D::TW3DComputePipelineState* mesh_bounding_box_calc_ps = new TW3D::TW3DComputePipelineState(rs0);
	mesh_bounding_box_calc_ps->SetShader(TW3DCompiledShader(CalculateMeshBoundingBox_ByteCode));
	mesh_bounding_box_calc_ps->Create(device);
	compute_shaders[CalculateMeshBoundingBox] = mesh_bounding_box_calc_ps;


	TW3D::TW3DRootSignature* rs1 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs1->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs1->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Bounding box Buffer SRV
	rs1->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer UAV
	rs1->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer UAV
	rs1->SetParameterConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs1->Create(device);

	TW3D::TW3DComputePipelineState* morton_calc_ps = new TW3D::TW3DComputePipelineState(rs1);
	morton_calc_ps->SetShader(TW3DCompiledShader(CalculateMortonCodes_ByteCode));
	morton_calc_ps->Create(device);
	compute_shaders[CalculateMortonCodes] = morton_calc_ps;


	TW3D::TW3DRootSignature* rs2 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs2->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs2->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer SRV
	rs2->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs2->SetParameterUAVBuffer(4, D3D12_SHADER_VISIBILITY_ALL, 1); // LBVH node lock buffer UAV to clear
	rs2->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	rs2->Create(device);

	TW3D::TW3DComputePipelineState* setup_lbvh_nodes_ps = new TW3D::TW3DComputePipelineState(rs2);
	setup_lbvh_nodes_ps->SetShader(TW3DCompiledShader(SetupLBVHNodes_ByteCode));
	setup_lbvh_nodes_ps->Create(device);
	compute_shaders[SetupLBVHNodes] = setup_lbvh_nodes_ps;


	TW3D::TW3DRootSignature* rs3 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs3->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer SRV
	rs3->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer SRV
	rs3->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs3->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs3->Create(device);

	TW3D::TW3DComputePipelineState* build_lbvh_splits_ps = new TW3D::TW3DComputePipelineState(rs3);
	build_lbvh_splits_ps->SetShader(TW3DCompiledShader(BuildLBVHSplits_ByteCode));
	build_lbvh_splits_ps->Create(device);
	compute_shaders[BuildLBVHSplits] = build_lbvh_splits_ps;


	TW3D::TW3DRootSignature* rs4 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs4->SetParameterUAVBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs4->SetParameterConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	rs4->Create(device);

	TW3D::TW3DComputePipelineState* update_lbvh_node_bounds = new TW3D::TW3DComputePipelineState(rs4);
	update_lbvh_node_bounds->SetShader(TW3DCompiledShader(UpdateLBVHNodeBounds_ByteCode));
	update_lbvh_node_bounds->Create(device);
	compute_shaders[UpdateLBVHNodeBounds] = update_lbvh_node_bounds;


	TW3D::TW3DRootSignature* rs5 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs5->SetParameterUAVBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global LBVH node buffer SRV
	rs5->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH node buffer SRV
	rs5->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs5->Create(device);

	TW3D::TW3DComputePipelineState* glbvh_nodes_ps = new TW3D::TW3DComputePipelineState(rs5);
	glbvh_nodes_ps->SetShader(TW3DCompiledShader(BuildGlobalLBVHNodeBuffer_ByteCode));
	glbvh_nodes_ps->Create(device);
	compute_shaders[BuildGLBVHNB] = glbvh_nodes_ps;


	TW3D::TW3DRootSignature* rs6 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs6->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Node Buffer SRV
	rs6->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Global Node Buffer offsets SRV
	rs6->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // Bounding box UAV
	rs6->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 3); // Input data constants
	rs6->Create(device);

	TW3D::TW3DComputePipelineState* lbvhs_bounding_box_calc_ps = new TW3D::TW3DComputePipelineState(rs6);
	lbvhs_bounding_box_calc_ps->SetShader(TW3DCompiledShader(CalculateLBVHsBoundingBox_ByteCode));
	lbvhs_bounding_box_calc_ps->Create(device);
	compute_shaders[CalculateLBVHsBoundingBox] = lbvhs_bounding_box_calc_ps;


	TW3D::TW3DRootSignature* rs7 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs7->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Node Buffer SRV
	rs7->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Global Node Buffer offsets SRV
	rs7->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Bounding box Buffer SRV
	rs7->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer UAV
	rs7->SetParameterUAVBuffer(4, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer UAV
	rs7->SetParameterConstants(5, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs7->Create(device);

	TW3D::TW3DComputePipelineState* lbvhs_morton_calc_ps = new TW3D::TW3DComputePipelineState(rs7);
	lbvhs_morton_calc_ps->SetShader(TW3DCompiledShader(CalculateMortonCodesForLBVHs_ByteCode));
	lbvhs_morton_calc_ps->Create(device);
	compute_shaders[CalculateMortonCodesForLBVHs] = lbvhs_morton_calc_ps;


	TW3D::TW3DRootSignature* rs8 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs8->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Node Buffer SRV
	rs8->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Global Node Buffer offsets SRV
	rs8->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Morton code indices buffer SRV
	rs8->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs8->SetParameterConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	rs8->Create(device);

	TW3D::TW3DComputePipelineState* lbvhs_setup_lbvh_nodes_ps = new TW3D::TW3DComputePipelineState(rs8);
	lbvhs_setup_lbvh_nodes_ps->SetShader(TW3DCompiledShader(SetupLBVHNodesFromLBVHs_ByteCode));
	lbvhs_setup_lbvh_nodes_ps->Create(device);
	compute_shaders[SetupLBVHNodesFromLBVHs] = lbvhs_setup_lbvh_nodes_ps;


	TW3D::TW3DRootSignature* rs9 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs9->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer SRV
	rs9->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer SRV
	rs9->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs9->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs9->Create(device);

	TW3D::TW3DComputePipelineState* lbvhs_build_lbvh_splits_ps = new TW3D::TW3DComputePipelineState(rs9);
	lbvhs_build_lbvh_splits_ps->SetShader(TW3DCompiledShader(BuildLBVHSplitsFromLBVHs_ByteCode));
	lbvhs_build_lbvh_splits_ps->Create(device);
	compute_shaders[BuildLBVHSplitsFromLBVHs] = lbvhs_build_lbvh_splits_ps;


	TW3D::TW3DRootSignature* rs10 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs10->SetParameterUAVBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs10->SetParameterConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs10->Create(device);

	TW3D::TW3DComputePipelineState* lbvhs_update_lbvh_node_bounds = new TW3D::TW3DComputePipelineState(rs10);
	lbvhs_update_lbvh_node_bounds->SetShader(TW3DCompiledShader(UpdateLBVHNodeBoundsForLBVHs_ByteCode));
	lbvhs_update_lbvh_node_bounds->Create(device);
	compute_shaders[UpdateLBVHNodeBoundsForLBVHs] = lbvhs_update_lbvh_node_bounds;


	// -----------------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------


	
}

void TW3DShaders::Release() {
	for (TW3D::TW3DComputePipelineState* ps : compute_shaders)
		delete ps;
	for (TW3D::TW3DGraphicsPipelineState* ps : graphics_shaders)
		delete ps;
}

TW3D::TW3DComputePipelineState* TW3DShaders::GetComputeShader(TWT::UInt Id) {
	return compute_shaders[Id];
}

TW3D::TW3DGraphicsPipelineState* TW3DShaders::GetGraphicsShader(TWT::UInt Id) {
	return graphics_shaders[Id];
}
