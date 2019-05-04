#include "pch.h"
#include "TW3DScene.h"
#include "TW3DShaders.h"

TW3D::TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager)
{
	Camera = new TW3DPerspectiveCamera(ResourceManager);

	gvb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(TWT::DefaultVertex));
	ResourceManager->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	gnb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(LBVHNode));

	LBVH = new TW3DLBVH(ResourceManager, 1, true);
}

TW3D::TW3DScene::~TW3DScene() {
	delete Camera;
	delete gvb;
	delete gnb;
	delete LBVH;
}

void TW3D::TW3DScene::Bind(TW3DGraphicsCommandList* CommandList, TWT::UInt GVBRPI, TWT::UInt SceneRTNBRPI, TWT::UInt GNBRPI) {
	CommandList->BindUAVSRV(GVBRPI, gvb);
	CommandList->BindUAVSRV(SceneRTNBRPI, LBVH->GetNodeBuffer());
	CommandList->BindUAVSRV(GNBRPI, gnb);
}

void TW3D::TW3DScene::AddObject(TW3DObject* Object) {
	Objects.push_back(Object);

	objects_changed = true;

	if (vertex_meshes.find(Object->VMInstance.VertexMesh) == vertex_meshes.end()) {
		vertex_meshes[Object->VMInstance.VertexMesh] = std::pair(-1, -1);
		mesh_buffers_changed = true;
	}

	for (TW3DVertexBuffer* vb : Object->VMInstance.VertexMesh->VertexBuffers)
		if (vertex_buffers.find(vb) == vertex_buffers.end()) {
			vertex_buffers[vb] = -1;
			vertex_buffers_changed = true;
		}
}

void TW3D::TW3DScene::RecordBeforeExecution() {
	// Collect vertex buffers & meshes
	// -------------------------------------------------------------------------------------------------------------------------
	TWT::UInt VertexOffset = 0;
	TWT::UInt NodeOffset = 0;
	mesh_instances.clear();

	if (vertex_buffers_changed) {
		for (auto& [vb, voffset] : vertex_buffers) {
			voffset = VertexOffset;
			VertexOffset += vb->GetVertexCount();
		}
		gvb_vertex_count = VertexOffset + 1;
	}

	if (mesh_buffers_changed) {
		for (auto& [mesh, moffsets] : vertex_meshes) {
			moffsets.first = vertex_buffers[mesh->VertexBuffers[0]];
			moffsets.second = NodeOffset;
			NodeOffset += mesh->GetLBVH()->GetNodeCount();
		}
		gnb_node_count = NodeOffset + 1;
	}

	if (objects_changed)
		for (TW3DObject* object : Objects) {
			std::pair<TWT::UInt, TWT::UInt> obj_mesh = vertex_meshes[object->VMInstance.VertexMesh];
			object->VMInstance.LBVHInstance.GVBOffset = obj_mesh.first;
			object->VMInstance.LBVHInstance.GNBOffset = obj_mesh.second;
			TWT::Matrix4f transform = object->VMInstance.Transform.GetModelMatrix();
			object->VMInstance.LBVHInstance.Transform = transform;
			object->VMInstance.LBVHInstance.TransformInverse = inverse(transform);

			mesh_instances.push_back(object->VMInstance.LBVHInstance);
		}


	if (vertex_buffers_changed) {
		auto ccl = resource_manager->GetTemporaryCopyCommandList();


		// Build Global Vertex Buffer
		// -------------------------------------------------------------------------------------------------------------------------
		for (auto entry : vertex_buffers)
			ccl->CopyBufferRegion(gvb, entry.second * sizeof(TWT::DefaultVertex), entry.first->GetResource(), 0, entry.first->GetSizeInBytes());

		ccl->Close();
		resource_manager->ExecuteCommandList(ccl);
		resource_manager->FlushCommandList(ccl);


		// Build LBVHs
		// -------------------------------------------------------------------------------------------------------------------------
		for (auto entry : vertex_meshes)
			entry.first->UpdateLBVH(gvb, entry.second.first);


		auto cl = resource_manager->GetTemporaryComputeCommandList();


		// Build Global Node Buffer
		// -------------------------------------------------------------------------------------------------------------------------
		cl->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::BuildGLBVHNB));
		cl->BindUAVBuffer(0, gnb);
		for (auto entry : vertex_meshes) {
			cl->BindUAVSRV(1, entry.first->GetLBVH()->GetNodeBuffer());
			cl->SetRoot32BitConstant(2, entry.second.second, 0);
			cl->Dispatch(entry.first->GetLBVH()->GetNodeCount());
			cl->ResourceBarrier(TW3DUAVBarrier());
		}
		cl->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);


		cl->Close();

		resource_manager->ExecuteCommandList(cl);
		resource_manager->FlushCommandList(cl);
	}

	if (objects_changed) {
		delete LBVH;
		LBVH = new TW3DLBVH(resource_manager, Objects.size(), true);
		LBVH->BuildFromLBVHs(gnb, mesh_instances);
	}

	vertex_buffers_changed = mesh_buffers_changed = objects_changed = false;
}
