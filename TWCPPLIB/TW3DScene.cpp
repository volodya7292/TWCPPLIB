#include "pch.h"
#include "TW3DScene.h"
#include "TW3DShaders.h"

TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager) {
	Camera = new TW3DPerspectiveCamera(ResourceManager);

	gvb = ResourceManager->CreateBuffer(1024, sizeof(TWT::DefaultVertex), true);
	ResourceManager->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	gnb = ResourceManager->CreateBuffer(1024, sizeof(LBVHNode), true);
	ResourceManager->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	LBVH = new TW3DLBVH(ResourceManager, 1, true);

	rp3d::WorldSettings settings = {};
	settings.isSleepingEnabled = false;

	// Create the world with your settings
	collision_world = new rp3d::DynamicsWorld(rp3d::Vector3(0, -9.81, 0));
	// Change the number of iterations of the velocity solver 
	collision_world->setNbIterationsVelocitySolver(15);
	// Change the number of iterations of the position solver 
	collision_world->setNbIterationsPositionSolver(8);
}

TW3DScene::~TW3DScene() {
	delete Camera;
	delete gvb;
	delete gnb;
	delete instance_buffer;
	delete LBVH;
	delete collision_world;
}

void TW3DScene::Bind(TW3DGraphicsCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI) {
	CommandList->BindBuffer(GVBRPI, gvb);
	CommandList->BindBuffer(SceneRTNBRPI, LBVH->GetNodeBuffer());
	CommandList->BindBuffer(GNBRPI, gnb);
}

void TW3DScene::AddObject(TW3DObject* Object) {
	Objects.push_back(Object);
	objects_changed = true;

	for (auto& vminst : Object->GetVertexMeshInstances()) {
		if (vertex_meshes.find(vminst.VertexMesh) == vertex_meshes.end()) {
			vertex_meshes[vminst.VertexMesh].gnb_offset = -1;
			vertex_meshes[vminst.VertexMesh].gvb_offset = -1;

			vminst.RigidBody = collision_world->createRigidBody(PhysicalTransform(vminst.Transform));

			//rp3d::Material& material = vminst.RigidBody->getMaterial();
   //         // Change the bounciness of the body 
			//material.setBounciness(rp3d::decimal(0.4));

			//// Change the friction coefficient of the body 
			//material.setFrictionCoefficient(rp3d::decimal(0.2));

			mesh_buffers_changed = true;
		}

		for (TW3DVertexBuffer* vb : vminst.VertexMesh->VertexBuffers)
			if (vertex_buffers.find(vb) == vertex_buffers.end()) {
				vertex_buffers[vb] = -1;
				vertex_buffers_changed = true;
			}
	}
}

void TW3DScene::Update(float DeltaTime) {
	for (TW3DObject* object : Objects) {
		object->Update();
	}
	
	collision_world->update(DeltaTime);
}

void TW3DScene::RecordBeforeExecution() {
	// Update vertex & meshes buffers
	// -------------------------------------------------------------------------------------------------------------------------
	if (vertex_buffers_changed) {
		TWT::uint VertexOffset = 0;

		for (auto& [vb, voffset] : vertex_buffers) {
			voffset = VertexOffset;
			VertexOffset += vb->GetVertexCount();
		}

		gvb_vertex_count = VertexOffset + 1;
	}

	if (mesh_buffers_changed) {
		TWT::uint NodeOffset = 0;

		for (auto& [mesh, moffsets] : vertex_meshes) {
			moffsets.gvb_offset = vertex_buffers[mesh->VertexBuffers[0]];
			moffsets.gnb_offset = NodeOffset;
			NodeOffset += mesh->GetLBVH()->GetNodeCount();
		}

		gnb_node_count = NodeOffset + 1;
	}

	mesh_instances.clear();
	mesh_instances.resize(Objects.size());
	for (size_t i = 0; i < Objects.size(); i++) {
		const auto& object = Objects[i];

		for (auto& vminst : object->GetVertexMeshInstances()) {
			const auto& obj_mesh = vertex_meshes[vminst.VertexMesh];

			vminst.LBVHInstance.GVBOffset = obj_mesh.gvb_offset;
			vminst.LBVHInstance.GNBOffset = obj_mesh.gnb_offset;

			if (vminst.Changed) {
				vminst.Changed = false;
				TWT::mat4 transform = vminst.Transform.GetModelMatrix();
				vminst.LBVHInstance.Transform = transform;
				vminst.LBVHInstance.TransformInverse = inverse(transform);
				objects_changed = true;
			}

			mesh_instances[i] = vminst.LBVHInstance;
		}
	}

	if (instance_buffer == nullptr || instance_buffer->GetElementCount() != Objects.size()) {
		instance_buffer = resource_manager->CreateBuffer(Objects.size(), sizeof(SceneLBVHInstance), true);
		resource_manager->ResourceBarrier(instance_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	}
	if (objects_changed)
		instance_buffer->Update(mesh_instances.data(), Objects.size());


	auto cl = resource_manager->GetTemporaryComputeCommandList();
	bool cl_updated = false;


	// Build Global Vertex Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	if (vertex_buffers_changed) {
		for (const auto& [vb, voffset]: vertex_buffers) {
			cl->CopyBufferRegion(gvb, voffset * sizeof(TWT::DefaultVertex), vb, 0, vb->GetSizeInBytes());
			cl_updated = true;
		}
	}


	// Build LBVHs
	// -------------------------------------------------------------------------------------------------------------------------
	for (const auto& [mesh, moffsets] : vertex_meshes) {
		mesh->UpdateLBVH(gvb, moffsets.gvb_offset, cl);
		cl_updated = true;
	}


	// Build Global Node Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	if (vertex_buffers_changed) {
		for (const auto& [mesh, moffsets] : vertex_meshes) {
			cl->CopyBufferRegion(gnb, moffsets.gnb_offset * sizeof(LBVHNode), mesh->GetLBVH()->GetNodeBuffer(), 0, mesh->GetLBVH()->GetNodeCount() * sizeof(LBVHNode));
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

rp3d::Transform TW3DScene::PhysicalTransform(TW3DTransform Transform) {
	TWT::vec3 position = Transform.GetPosition();
	TWT::vec3 rotation = Transform.GetRotation();

	rp3d::Vector3 initPosition(position.x, position.y, position.z);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(rotation.x, rotation.y, rotation.z));

	return rp3d::Transform(initPosition, initOrientation);
}