#include "pch.h"
#include "TW3DShaders.h"
#include "CalculateMeshBoundingBox.c.h"
#include "CalculateMortonCodes.c.h"
#include "SetupLBVHNodes.c.h"
#include "BuildLBVHSplits.c.h"
#include "UpdateLBVHNodeBoundaries.c.h"

TWT::Vector<TW3D::TW3DComputePipelineState*> compute_shaders;
TWT::Vector<TW3D::TW3DGraphicsPipelineState*> graphics_shaders;

void TW3DShaders::Initialize(TW3D::TW3DResourceManager* ResourceManager) {
	compute_shaders.resize(ComputeShaderCount);

	TW3D::TW3DDevice* device = ResourceManager->GetDevice();

	TW3D::TW3DRootSignature* rs0 = new TW3D::TW3DRootSignature(false, false, false, false);
	rs0->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs0->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0); // Bounding box UAV
	rs0->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4); // Input data constants
	rs0->Create(device);

	TW3D::TW3DComputePipelineState* bounding_box_calc_ps = new TW3D::TW3DComputePipelineState(rs0);
	bounding_box_calc_ps->SetShader(TW3DCompiledShader(CalculateMeshBoundingBox_ByteCode));
	bounding_box_calc_ps->Create(device);
	compute_shaders[CalculateMeshBoundingBox] = bounding_box_calc_ps;


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
	rs2->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton codes buffer SRV
	rs2->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Morton code indices buffer SRV
	rs2->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	//rs2->SetParameterUAVBuffer(4, D3D12_SHADER_VISIBILITY_ALL, 1); // LBVH node lock buffer UAV to clear
	rs2->SetParameterConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
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
	rs4->SetParameterConstants(1, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs4->Create(device);

	TW3D::TW3DComputePipelineState* update_lbvh_node_boundaries = new TW3D::TW3DComputePipelineState(rs4);
	update_lbvh_node_boundaries->SetShader(TW3DCompiledShader(UpdateLBVHNodeBoundaries_ByteCode));
	update_lbvh_node_boundaries->Create(device);
	compute_shaders[UpdateLBVHNodeBounds] = update_lbvh_node_boundaries;
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
