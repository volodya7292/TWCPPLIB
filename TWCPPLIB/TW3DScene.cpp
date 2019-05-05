#include "pch.h"
#include "TW3DScene.h"
#include "TW3DShaders.h"

TW3D::TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager) {
	Camera = new TW3DPerspectiveCamera(ResourceManager);

	gvb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(TWT::DefaultVertex));
	ResourceManager->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	gnb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(LBVHNode));
	ResourceManager->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	LBVH = new TW3DLBVH(ResourceManager, 1, true);
}

TW3D::TW3DScene::~TW3DScene() {
	delete Camera;
	delete gvb;
	delete gnb;
	delete instance_buffer;
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
	// Update vertex & meshes buffers
	// -------------------------------------------------------------------------------------------------------------------------
	if (vertex_buffers_changed) {
		TWT::UInt VertexOffset = 0;

		for (auto& [vb, voffset] : vertex_buffers) {
			voffset = VertexOffset;
			VertexOffset += vb->GetVertexCount();
		}

		gvb_vertex_count = VertexOffset + 1;
	}

	if (mesh_buffers_changed) {
		TWT::UInt NodeOffset = 0;

		for (auto& [mesh, moffsets] : vertex_meshes) {
			moffsets.first = vertex_buffers[mesh->VertexBuffers[0]];
			moffsets.second = NodeOffset;
			NodeOffset += mesh->GetLBVH()->GetNodeCount();
		}

		gnb_node_count = NodeOffset + 1;
	}

	mesh_instances.clear();
	mesh_instances.resize(Objects.size());
	for (size_t i = 0; i < Objects.size(); i++) {
		const auto& object = Objects[i];
		const std::pair<TWT::UInt, TWT::UInt>& obj_mesh = vertex_meshes[object->VMInstance.VertexMesh];

		object->VMInstance.LBVHInstance.GVBOffset = obj_mesh.first;
		object->VMInstance.LBVHInstance.GNBOffset = obj_mesh.second;

		if (object->VMInstance.Changed) {
			object->VMInstance.Changed = false;
			TWT::Matrix4f transform = object->VMInstance.Transform.GetModelMatrix();
			object->VMInstance.LBVHInstance.Transform = transform;
			object->VMInstance.LBVHInstance.TransformInverse = inverse(transform);
			objects_changed = true;
		}

		mesh_instances[i] = object->VMInstance.LBVHInstance;
	}

	if (instance_buffer == nullptr || instance_buffer->GetElementCount() != Objects.size()) {
		instance_buffer = resource_manager->CreateUnorderedAccessView(Objects.size(), sizeof(SceneLBVHInstance));
		resource_manager->ResourceBarrier(instance_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	}
	if (objects_changed)
		instance_buffer->UpdateData(mesh_instances.data(), Objects.size());


	auto cl = resource_manager->GetTemporaryComputeCommandList();
	bool cl_updated = false;


	// Build Global Vertex Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	if (vertex_buffers_changed) {
		for (const auto& [vb, voffset]: vertex_buffers) {
			cl->CopyBufferRegion(gvb, voffset * sizeof(TWT::DefaultVertex), vb->GetResource(), 0, vb->GetSizeInBytes());
			cl_updated = true;
		}
	}


	// Build LBVHs
	// -------------------------------------------------------------------------------------------------------------------------
	for (const auto& [mesh, moffsets] : vertex_meshes) {
		mesh->UpdateLBVH(gvb, moffsets.first, cl);
		cl_updated = true;
	}


	// Build Global Node Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	if (vertex_buffers_changed) {
		for (const auto& [mesh, moffsets] : vertex_meshes) {
			cl->CopyBufferRegion(gnb, moffsets.second * sizeof(LBVHNode), mesh->GetLBVH()->GetNodeBuffer(), 0, mesh->GetLBVH()->GetNodeCount() * sizeof(LBVHNode));
			cl_updated = true;
		}
	}


	// Build scene LBVH
	// -------------------------------------------------------------------------------------------------------------------------
	if (objects_changed) {
		LBVH->BuildFromLBVHs(gnb, instance_buffer, cl);
		cl_updated = true;
	}


	cl->Close();
	if (cl_updated) {
		resource_manager->ExecuteCommandList(cl);
		resource_manager->FlushCommandList(cl);
	}

	vertex_buffers_changed = mesh_buffers_changed = objects_changed = false;
}
