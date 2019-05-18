#include "pch.h"
#include "TW3DLBVH.h"
#include "TW3DShaders.h"
#include "TW3DModules.h"

TW3DLBVH::TW3DLBVH(TW3DResourceManager* ResourceManager, TWT::uint ElementCount, bool SceneLevel) :
	resource_manager(ResourceManager), element_count(ElementCount) {
	morton_codes_buffer = resource_manager->CreateUnorderedAccessView(element_count, sizeof(LBVHMortonCode));
	//morton_indices_buffer = resource_manager->CreateUnorderedAccessView(element_count, sizeof(TWT::uint));
	bounding_box_buffer = resource_manager->CreateUnorderedAccessView(element_count, sizeof(TWT::Bounds));
	node_buffer = resource_manager->CreateUnorderedAccessView(GetNodeCount(), SceneLevel ? sizeof(SceneLBVHNode) : sizeof(LBVHNode));
	resource_manager->ResourceBarrier(node_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);
	resource_manager->ResourceBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE);
}

TW3DLBVH::~TW3DLBVH() {
	delete morton_codes_buffer;
	//delete morton_indices_buffer;
	delete bounding_box_buffer;
	delete node_buffer;
}

TWT::uint TW3DLBVH::GetNodeCount() {
	return 2 * element_count - 1;
}

TW3DResourceUAV* TW3DLBVH::GetNodeBuffer() {
	return node_buffer;
}

void TW3DLBVH::BuildFromTriangles(TW3DResourceUAV* GVB, TWT::uint GVBOffset, TW3DGraphicsCommandList* CommandList) {
	TW3DGraphicsCommandList* cl;
	if (CommandList)
		cl = CommandList;
	else
		cl = resource_manager->GetTemporaryComputeCommandList();

	auto uav_barrier = TW3DUAVBarrier();

	// Calculate bounding box
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::CalculateTriangleMeshBoundingBox));
	//cl->ResourceBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cl->BindUAVSRV(0, GVB);
	cl->BindUAVBuffer(1, bounding_box_buffer);
	cl->SetRoot32BitConstant(2, GVBOffset, 0);
	cl->SetRoot32BitConstant(2, element_count * 3, 1);
	int element_count2 = element_count;
	TWT::uint iteration = 0;
	do {
		cl->SetRoot32BitConstant(2, iteration, 2);
		cl->SetRoot32BitConstant(2, element_count2, 3);

		element_count2 = ceil(element_count2 / 16.0f);
		cl->Dispatch(ceil(element_count2 / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		iteration++;
	} while (element_count2 > 1);

	if (element_count == 1) {
		cl->ResourceBarriers({
			//TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE),
			//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
		});

		cl->CopyBufferRegion(node_buffer, 0, bounding_box_buffer, 0, sizeof(TWT::Bounds));

		cl->ResourceBarriers({
			TW3DUAVBarrier(),
			//TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COPY_SOURCE)
		});
	} else {
		cl->ResourceBarriers({
			//TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			TW3DTransitionBarrier(morton_codes_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
			//TW3DTransitionBarrier(morton_indices_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		});


		// Calculate morton codes
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::CalculateMortonCodesForTriangles));
		cl->BindUAVSRV(0, GVB);
		cl->BindUAVSRV(1, bounding_box_buffer);
		cl->BindUAVBuffer(2, morton_codes_buffer);
		//cl->BindUAVBuffer(3, morton_indices_buffer);
		cl->SetRoot32BitConstant(3, GVBOffset, 0);
		cl->SetRoot32BitConstant(3, element_count * 3, 1);
		cl->Dispatch(ceil(element_count / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		// Sort morton codes
		TW3DModules::BitonicSorter()->RecordSort(cl, morton_codes_buffer, element_count, true, false);

		cl->ResourceBarriers({
			TW3DTransitionBarrier(morton_codes_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			//TW3DTransitionBarrier(morton_indices_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		});

		// Setup LBVH nodes
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::SetupLBVHNodesFromTriangles));
		cl->BindUAVSRV(0, GVB);
		cl->BindUAVSRV(1, morton_codes_buffer);
		//cl->BindUAVSRV(1, morton_indices_buffer);
		cl->BindUAVBuffer(2, node_buffer);
		//cl->BindUAVBuffer(4, VertexMesh->GetLBVHNodeLockBufferResource());
		cl->SetRoot32BitConstant(3, GVBOffset, 0);
		cl->SetRoot32BitConstant(3, element_count - 1, 1);
		cl->SetRoot32BitConstant(3, GetNodeCount(), 2);
		cl->Dispatch(ceil(GetNodeCount() / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		// Build LBVH splits
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::BuildLBVHSplits));
		cl->BindUAVSRV(0, morton_codes_buffer);
		cl->BindUAVBuffer(1, node_buffer);
		cl->SetRoot32BitConstant(2, element_count, 0);
		cl->SetRoot32BitConstant(2, element_count - 1, 1);
		cl->Dispatch(ceil((element_count - 1) / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		// Update LVBH node bounds
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::UpdateLBVHNodeBounds));
		cl->BindUAVBuffer(0, node_buffer);
		cl->SetRoot32BitConstant(1, element_count - 1, 0);
		cl->SetRoot32BitConstant(1, GetNodeCount(), 1);
		cl->Dispatch(ceil(element_count / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		//cl->ResourceBarriers({
		//	//TW3DUAVBarrier(),
		//	//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE)
		//});
	}

	if (!CommandList) {
		cl->Close();
		resource_manager->ExecuteCommandList(cl);
		resource_manager->FlushCommandList(cl);
	}
}

void TW3DLBVH::BuildFromLBVHs(TW3DResourceUAV* GNB, TW3DResourceUAV* SceneMeshInstancesBuffer, TW3DGraphicsCommandList* CommandList) {
	TWT::uint gnboffset_count = SceneMeshInstancesBuffer->GetElementCount();
	bool partially_presorted = true;

	if (gnboffset_count != element_count) {
		partially_presorted = false;
		element_count = gnboffset_count;
		delete morton_codes_buffer;
		//delete morton_indices_buffer;
		delete bounding_box_buffer;
		delete node_buffer;
		morton_codes_buffer = resource_manager->CreateUnorderedAccessView(element_count, sizeof(LBVHMortonCode));
		//morton_indices_buffer = resource_manager->CreateUnorderedAccessView(element_count, sizeof(TWT::uint));
		bounding_box_buffer = resource_manager->CreateUnorderedAccessView(element_count, sizeof(TWT::Bounds));
		node_buffer = resource_manager->CreateUnorderedAccessView(GetNodeCount(), sizeof(SceneLBVHNode));
	}

	//TW3DResourceUAV* gnboffset_buffer = resource_manager->CreateUnorderedAccessView(gnboffset_count, sizeof(SceneLBVHInstance));
	//gnboffset_buffer->UpdateData(MeshLBVHInstances.data(), gnboffset_count);


	TW3DGraphicsCommandList* cl;
	if (CommandList)
		cl = CommandList;
	else
		cl = resource_manager->GetTemporaryComputeCommandList();

	auto uav_barrier = TW3DUAVBarrier();

	// Calculate bounding box
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::CalculateLBVHsBoundingBox));
	//cl->ResourceBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cl->BindUAVSRV(0, GNB);
	cl->BindUAVSRV(1, SceneMeshInstancesBuffer);
	cl->BindUAVBuffer(2, bounding_box_buffer);
	cl->SetRoot32BitConstant(3, gnboffset_count, 0);
	int element_count2 = element_count;
	TWT::uint iteration = 0;
	do {
		cl->SetRoot32BitConstant(3, iteration, 1);
		cl->SetRoot32BitConstant(3, element_count2, 2);

		element_count2 = ceil(element_count2 / 16.0f);
		cl->Dispatch(ceil(element_count2 / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		iteration++;
	} while (element_count2 > 1);

	if (element_count == 1) {
		cl->ResourceBarriers({
			TW3DTransitionBarrier(SceneMeshInstancesBuffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE),
			//TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE),
			//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST)
		});

		cl->CopyBufferRegion(node_buffer, offsetof(SceneLBVHNode, instance), SceneMeshInstancesBuffer, 0, sizeof(SceneLBVHInstance));
		cl->CopyBufferRegion(node_buffer, offsetof(SceneLBVHNode, bounds), bounding_box_buffer, 0, sizeof(TWT::Bounds));

		cl->ResourceBarriers({
			//TW3DUAVBarrier(),
			TW3DTransitionBarrier(SceneMeshInstancesBuffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			//TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
			//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		});
	} else {
		//cl->ResourceBarriers({
		//	//TW3DTransitionBarrier(bounding_box_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		//	//TW3DTransitionBarrier(morton_codes_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS),
		//	//TW3DTransitionBarrier(morton_indices_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		//});

		// Calculate morton codes
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::CalculateMortonCodesForLBVHs));
		cl->BindUAVSRV(0, GNB);
		cl->BindUAVSRV(1, SceneMeshInstancesBuffer);
		cl->BindUAVSRV(2, bounding_box_buffer);
		cl->BindUAVBuffer(3, morton_codes_buffer);
		//cl->BindUAVBuffer(4, morton_indices_buffer);
		cl->SetRoot32BitConstant(4, gnboffset_count, 0);
		cl->Dispatch(ceil(element_count / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		// Sort morton codes
		TW3DModules::BitonicSorter()->RecordSort(cl, morton_codes_buffer, element_count, true, partially_presorted);

		//cl->ResourceBarriers({
		//	//TW3DTransitionBarrier(morton_codes_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		//	//TW3DTransitionBarrier(morton_indices_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
		//	//TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		//});

		// Setup LBVH nodes
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::SetupLBVHNodesFromLBVHs));
		cl->BindUAVSRV(0, GNB);
		cl->BindUAVSRV(1, SceneMeshInstancesBuffer);
		cl->BindUAVSRV(2, morton_codes_buffer);
		cl->BindUAVBuffer(3, node_buffer);
		cl->SetRoot32BitConstant(4, gnboffset_count, 0);
		cl->SetRoot32BitConstant(4, element_count - 1, 1);
		cl->SetRoot32BitConstant(4, GetNodeCount(), 2);
		cl->Dispatch(ceil(GetNodeCount() / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		// Build LBVH splits
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::BuildLBVHSplitsFromLBVHs));
		cl->BindUAVSRV(0, morton_codes_buffer);
		cl->BindUAVBuffer(1, node_buffer);
		cl->SetRoot32BitConstant(2, element_count, 0);
		cl->SetRoot32BitConstant(2, element_count - 1, 1);
		cl->Dispatch(ceil((element_count - 1) / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		// Update LVBH node boundaries
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::UpdateLBVHNodeBoundsForLBVHs));
		cl->BindUAVBuffer(0, node_buffer);
		cl->SetRoot32BitConstant(1, element_count - 1, 0);
		cl->SetRoot32BitConstant(1, GetNodeCount(), 1);
		cl->Dispatch(ceil(element_count / 64.0f));
		cl->ResourceBarrier(uav_barrier);

		/*cl->ResourceBarriers({
			TW3DTransitionBarrier(node_buffer, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		});*/
	}

	if (!CommandList) {
		cl->Close();
		resource_manager->ExecuteCommandList(cl);
		resource_manager->FlushCommandList(cl);
	}
}
