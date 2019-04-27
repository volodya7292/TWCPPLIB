#include "pch.h"
#include "TW3DLBVH.h"
#include "TW3DShaders.h"
#include "TW3DModules.h"

TW3D::TW3DLBVH::TW3DLBVH(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager) {
}

TW3D::TW3DLBVH::~TW3DLBVH() {
}

TWT::UInt TW3D::TW3DLBVH::GetNodeCount() {
	return 2 * primitive_count - 1;
}

TW3D::TW3DResourceUAV* TW3D::TW3DLBVH::GetNodeBuffer() {
	return node_buffer;
}

void TW3D::TW3DLBVH::BuildFromPrimitives(TW3DResourceUAV* GVB, TWT::UInt GVBOffset, TWT::UInt PrimitiveCount) {
	primitive_count = PrimitiveCount;

	delete morton_codes_buffer;
	delete morton_indices_buffer;
	delete bounding_box_buffer;
	delete node_buffer;
	morton_codes_buffer = resource_manager->CreateUnorderedAccessView(PrimitiveCount, sizeof(TWT::UInt));
	morton_indices_buffer = resource_manager->CreateUnorderedAccessView(PrimitiveCount, sizeof(TWT::UInt));
	bounding_box_buffer = resource_manager->CreateUnorderedAccessView(PrimitiveCount, sizeof(TWT::Bounds));
	node_buffer = resource_manager->CreateUnorderedAccessView(GetNodeCount(), sizeof(TWT::LBVHNode));

	auto cl = resource_manager->GetTemporaryComputeCommandList();


	auto uav_barrier = TW3DUAVBarrier();
	cl->Reset();

	// Calculate bounding box
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::CalculateMeshBoundingBox));
	cl->ResourceBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cl->BindUAVBufferSRV(0, GVB);
	cl->BindUAVBuffer(1, bounding_box_buffer);
	cl->SetRoot32BitConstant(2, GVBOffset, 0);
	cl->SetRoot32BitConstant(2, PrimitiveCount * 3, 1);
	int element_count = PrimitiveCount;
	TWT::UInt iteration = 0;
	do {
		cl->SetRoot32BitConstant(2, iteration, 2);
		cl->SetRoot32BitConstant(2, element_count, 3);

		element_count = ceil(element_count / 16.0f);
		cl->Dispatch(element_count);
		cl->ResourceBarrier(uav_barrier);

		iteration++;
	} while (element_count > 1);

	cl->ResourceBarriers({
		TW3D::TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3D::TW3DTransitionBarrier(morton_codes_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		TW3D::TW3DTransitionBarrier(morton_indices_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	});

	// Calculate morton codes
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::CalculateMortonCodes));
	cl->BindUAVBufferSRV(0, GVB);
	cl->BindUAVBufferSRV(1, bounding_box_buffer);
	cl->BindUAVBuffer(2, morton_codes_buffer);
	cl->BindUAVBuffer(3, morton_indices_buffer);
	cl->SetRoot32BitConstant(4, GVBOffset, 0);
	cl->Dispatch(PrimitiveCount);
	cl->ResourceBarrier(uav_barrier);

	// Sort morton codes
	TW3DModules::BitonicSorter()->RecordSort(cl, morton_codes_buffer, morton_indices_buffer, PrimitiveCount, true);

	cl->ResourceBarriers({
		TW3D::TW3DTransitionBarrier(morton_codes_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3D::TW3DTransitionBarrier(morton_indices_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		TW3D::TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	});

		// Setup LBVH nodes
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::SetupLBVHNodes));
	cl->BindUAVBufferSRV(0, GVB);
	cl->BindUAVBufferSRV(1, morton_codes_buffer);
	cl->BindUAVBufferSRV(2, morton_indices_buffer);
	cl->BindUAVBuffer(3, node_buffer);
	//cl->BindUAVBuffer(4, VertexMesh->GetLBVHNodeLockBufferResource());
	cl->SetRoot32BitConstant(4, GVBOffset, 0);
	cl->SetRoot32BitConstant(4, PrimitiveCount - 1, 1);
	cl->Dispatch(2 * PrimitiveCount - 1);
	cl->ResourceBarrier(uav_barrier);

	// Build LBVH splits
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::BuildLBVHSplits));
	cl->BindUAVBufferSRV(0, morton_codes_buffer);
	cl->BindUAVBufferSRV(1, morton_indices_buffer);
	cl->BindUAVBuffer(2, node_buffer);
	cl->SetRoot32BitConstant(3, PrimitiveCount, 0);
	cl->Dispatch(PrimitiveCount - 1);
	cl->ResourceBarrier(uav_barrier);

	// Update LVBH node boundaries
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::UpdateLBVHNodeBounds));
	cl->BindUAVBuffer(0, node_buffer);
	cl->SetRoot32BitConstant(1, PrimitiveCount - 1, 0);
	cl->Dispatch(PrimitiveCount);

	cl->ResourceBarriers({
		TW3D::TW3DUAVBarrier(),
		TW3D::TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
	});

	cl->Close();

	resource_manager->ExecuteCommandList(cl);
	resource_manager->FlushCommandList(cl);
}

void TW3D::TW3DLBVH::BuildFromLBVHs(TW3DResourceUAV * GNB, TWT::UInt GNBOffset, TWT::UInt NodeCount) {
}
