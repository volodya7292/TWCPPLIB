#include "pch.h"
#include "TW3DLBVHBuilder.h"
#include "CalculateMeshBoundingBox.c.h"
#include "CalculateMortonCodes.c.h"
#include "SetupLBVHNodes.c.h"
#include "BuildLBVHSplits.c.h"
#include "UpdateLBVHNodeBoundaries.c.h"

TW3D::TW3DLBVHBuilder::TW3DLBVHBuilder(TW3DResourceManager* ResourceManager) {
	bitonic_sorter = new TW3DBitonicSorter(ResourceManager);

	TW3DDevice* device = ResourceManager->GetDevice();

	TW3DRootSignature* rs0 = new TW3DRootSignature(false, false, false, false);
	rs0->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs0->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0); // Bounding box UAV
	rs0->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4); // Input data constants
	rs0->Create(device);

	bounding_box_calc_ps = new TW3DComputePipelineState(rs0);
	bounding_box_calc_ps->SetShader(TW3DCompiledShader(CalculateMeshBoundingBox_ByteCode));
	bounding_box_calc_ps->Create(device);


	TW3DRootSignature* rs1 = new TW3DRootSignature(false, false, false, false);
	rs1->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs1->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Bounding box Buffer SRV
	rs1->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer UAV
	rs1->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer UAV
	rs1->SetParameterConstants(4, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs1->Create(device);

	morton_calc_ps = new TW3DComputePipelineState(rs1);
	morton_calc_ps->SetShader(TW3DCompiledShader(CalculateMortonCodes_ByteCode));
	morton_calc_ps->Create(device);


	TW3DRootSignature* rs2 = new TW3DRootSignature(false, false, false, false);
	rs2->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Global Vertex Buffer SRV
	rs2->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton codes buffer SRV
	rs2->SetParameterSRV(2, D3D12_SHADER_VISIBILITY_ALL, 2); // Morton code indices buffer SRV
	rs2->SetParameterUAVBuffer(3, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs2->SetParameterUAVBuffer(4, D3D12_SHADER_VISIBILITY_ALL, 1); // LBVH node lock buffer UAV to clear
	rs2->SetParameterConstants(5, D3D12_SHADER_VISIBILITY_ALL, 0, 2); // Input data constants
	rs2->Create(device);

	setup_lbvh_nodes_ps = new TW3DComputePipelineState(rs2);
	setup_lbvh_nodes_ps->SetShader(TW3DCompiledShader(SetupLBVHNodes_ByteCode));
	setup_lbvh_nodes_ps->Create(device);


	TW3DRootSignature* rs3 = new TW3DRootSignature(false, false, false, false);
	rs3->SetParameterSRV(0, D3D12_SHADER_VISIBILITY_ALL, 0); // Morton codes buffer SRV
	rs3->SetParameterSRV(1, D3D12_SHADER_VISIBILITY_ALL, 1); // Morton code indices buffer SRV
	rs3->SetParameterUAVBuffer(2, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs3->SetParameterConstants(3, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs3->Create(device);

	build_lbvh_splits_ps = new TW3DComputePipelineState(rs3);
	build_lbvh_splits_ps->SetShader(TW3DCompiledShader(BuildLBVHSplits_ByteCode));
	build_lbvh_splits_ps->Create(device);


	TW3DRootSignature* rs4 = new TW3DRootSignature(false, false, false, false);
	rs4->SetParameterUAVBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0); // LBVH nodes buffer UAV
	rs4->SetParameterUAVBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 1); // LBVH node locks buffer UAV
	rs4->SetParameterConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 1); // Input data constants
	rs4->Create(device);

	update_lbvh_node_boundaries = new TW3DComputePipelineState(rs4);
	update_lbvh_node_boundaries->SetShader(TW3DCompiledShader(UpdateLBVHNodeBoundaries_ByteCode));
	update_lbvh_node_boundaries->Create(device);
}

TW3D::TW3DLBVHBuilder::~TW3DLBVHBuilder() {
	delete bitonic_sorter;
	delete bounding_box_calc_ps;
	delete morton_calc_ps;
	delete setup_lbvh_nodes_ps;
	delete build_lbvh_splits_ps;
	delete update_lbvh_node_boundaries;
}

void TW3D::TW3DLBVHBuilder::Build(TW3DGraphicsCommandList* CommandList, TW3DResourceUAV* GVB, TW3DVertexMesh* VertexMesh) {
	auto uav_barrier = TW3DUAVBarrier();

	// Calculate bounding box
	CommandList->SetPipelineState(bounding_box_calc_ps);
	CommandList->ResourceBarrier(VertexMesh->GetBBBufferResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	CommandList->BindUAVBufferSRV(0, GVB);
	CommandList->BindUAVBuffer(1, VertexMesh->GetBBBufferResource());
	CommandList->SetRoot32BitConstant(2, VertexMesh->GetGVBVertexOffset(), 0);
	CommandList->SetRoot32BitConstant(2, VertexMesh->GetVertexCount(), 1);
	int element_count = VertexMesh->GetTriangleCount();
	TWT::UInt iteration = 0;
	do {
		CommandList->SetRoot32BitConstant(2, iteration, 2);
		CommandList->SetRoot32BitConstant(2, element_count, 3);

		element_count = ceil(element_count / 16.0f);
		CommandList->Dispatch(element_count);
		CommandList->ResourceBarrier(uav_barrier);

		iteration++;
	} while (element_count > 1);

	CommandList->ResourceBarriers({
		TW3D::TW3DTransitionBarrier(VertexMesh->GetBBBufferResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3D::TW3DTransitionBarrier(VertexMesh->GetMCBufferResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3D::TW3DTransitionBarrier(VertexMesh->GetMCIBufferResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	});

	// Calculate morton codes
	CommandList->SetPipelineState(morton_calc_ps);
	CommandList->BindUAVBufferSRV(0, GVB);
	CommandList->BindUAVBufferSRV(1, VertexMesh->GetBBBufferResource());
	CommandList->BindUAVBuffer(2, VertexMesh->GetMCBufferResource());
	CommandList->BindUAVBuffer(3, VertexMesh->GetMCIBufferResource());
	CommandList->SetRoot32BitConstant(4, VertexMesh->GetGVBVertexOffset(), 0);
	CommandList->Dispatch(VertexMesh->GetTriangleCount());
	CommandList->ResourceBarrier(uav_barrier);

	// Sort morton codes
	bitonic_sorter->RecordSort(CommandList, VertexMesh->GetMCBufferResource(), VertexMesh->GetMCIBufferResource(), VertexMesh->GetTriangleCount(), true);

	CommandList->ResourceBarriers({
		TW3D::TW3DTransitionBarrier(VertexMesh->GetMCBufferResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3D::TW3DTransitionBarrier(VertexMesh->GetMCIBufferResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3D::TW3DTransitionBarrier(VertexMesh->GetLBVHNodeBufferResource(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	});

	// Setup LBVH nodes
	CommandList->SetPipelineState(setup_lbvh_nodes_ps);
	CommandList->BindUAVBufferSRV(0, GVB);
	CommandList->BindUAVBufferSRV(1, VertexMesh->GetMCBufferResource());
	CommandList->BindUAVBufferSRV(2, VertexMesh->GetMCIBufferResource());
	CommandList->BindUAVBuffer(3, VertexMesh->GetLBVHNodeBufferResource());
	CommandList->BindUAVBuffer(4, VertexMesh->GetLBVHNodeLockBufferResource());
	CommandList->SetRoot32BitConstant(5, VertexMesh->GetGVBVertexOffset(), 0);
	CommandList->SetRoot32BitConstant(5, VertexMesh->GetTriangleCount() - 1, 1);
	CommandList->Dispatch(2 * VertexMesh->GetTriangleCount() - 1);
	CommandList->ResourceBarrier(uav_barrier);

	// Build LBVH splits
	CommandList->SetPipelineState(build_lbvh_splits_ps);
	CommandList->BindUAVBufferSRV(0, VertexMesh->GetMCBufferResource());
	CommandList->BindUAVBufferSRV(1, VertexMesh->GetMCIBufferResource());
	CommandList->BindUAVBuffer(2, VertexMesh->GetLBVHNodeBufferResource());
	CommandList->SetRoot32BitConstant(3, VertexMesh->GetTriangleCount(), 0);
	CommandList->Dispatch(VertexMesh->GetTriangleCount() - 1);
	CommandList->ResourceBarrier(uav_barrier);

	// Update LVBH node boundaries
	CommandList->SetPipelineState(update_lbvh_node_boundaries);
	CommandList->BindUAVBuffer(0, VertexMesh->GetLBVHNodeBufferResource());
	CommandList->BindUAVBuffer(1, VertexMesh->GetLBVHNodeLockBufferResource());
	CommandList->SetRoot32BitConstant(2, VertexMesh->GetTriangleCount() - 1, 0);
	CommandList->Dispatch(VertexMesh->GetTriangleCount());

	CommandList->ResourceBarriers({
		TW3D::TW3DUAVBarrier(),
		TW3D::TW3DTransitionBarrier(VertexMesh->GetLBVHNodeBufferResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
	});
}