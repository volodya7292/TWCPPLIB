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

	lsb = ResourceManager->CreateBuffer(1024, sizeof(TW3DSceneLightSource), true);
	ResourceManager->ResourceBarrier(lsb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

	LBVH = new TW3DLBVH(ResourceManager, 1, true);

	rp3d::WorldSettings settings = {};
	settings.isSleepingEnabled = false;

	// Create the world with your settings
	collision_world = new rp3d::DynamicsWorld(rp3d::Vector3(0, -9.81, 0));
	collision_world->enableSleeping(true);
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
	delete lsb;
	delete LBVH;
	delete collision_world;
}

void TW3DScene::Bind(TW3DCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI, TWT::uint LSBRPI) {
	CommandList->BindBuffer(GVBRPI, gvb);
	CommandList->BindBuffer(SceneRTNBRPI, LBVH->GetNodeBuffer());
	CommandList->BindBuffer(GNBRPI, gnb);
	CommandList->BindBuffer(LSBRPI, lsb);
}

void TW3DScene::AddObject(TW3DObject* Object) {
	Objects.push_back(Object);
	objects_changed = true;

	for (auto& vminst : Object->GetVertexMeshInstances()) {
		if (vertex_meshes.find(vminst.VertexMesh) == vertex_meshes.end()) {
			vertex_meshes[vminst.VertexMesh].gnb_offset = -1;
			vertex_meshes[vminst.VertexMesh].gvb_offset = -1;

			auto d = vminst.Transform.GetPhysicalTransform();

			TW3DTransform d2;
			d2.InitFromPhysicalTransform(d);

			vminst.RigidBody = collision_world->createRigidBody(d);

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

void TW3DScene::AddLightSource(TW3DLightSource* LightSource) {
	LightSources.push_back(LightSource);
}

void TW3DScene::Update(float DeltaTime) {
	for (TW3DObject* object : Objects) {
		object->Update();
	}

	for (TWT::uint i = 0; i < LightSources.size(); i++) {
		TW3DLightSource* light = LightSources[i];

		if (light->Updated || offsets_updated) {
			light->Updated = false;

			TW3DSceneLightSource ls;
			ls.position = TWT::float4(light->GetPosition(), 1);
			ls.color = TWT::float4(light->GetColor(), 1);
			ls.info = light->MakeInfo();
			if (light->Type == TW3D_LIGHT_SOURCE_TRIANGLE)
				ls.info.y += vertex_buffers[light->GetTriangleVertexBuffer()] / 3;

			lsb->UpdateElement(&ls, i);
		}
	}
	
	collision_world->update(DeltaTime);
}

void TW3DScene::BeforeExecution() {
	TWT::uint object_count = Objects.size();

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
	mesh_instances.resize(object_count);
	for (size_t i = 0; i < object_count; i++) {
		const auto& object = Objects[i];

		for (auto& vminst : object->GetVertexMeshInstances()) {
			const auto& obj_mesh = vertex_meshes[vminst.VertexMesh];

			vminst.LBVHInstance.GVBOffset = obj_mesh.gvb_offset;
			vminst.LBVHInstance.GNBOffset = obj_mesh.gnb_offset;

			if (vminst.Changed) {
				vminst.Changed = false;
				TWT::float4x4 transform = vminst.Transform.GetModelMatrix();
				vminst.LBVHInstance.Transform = transform;
				vminst.LBVHInstance.TransformInverse = inverse(transform);
				objects_changed = true;
			}

			mesh_instances[i] = vminst.LBVHInstance;
		}
	}

	if ((!instance_buffer || instance_buffer->GetElementCount() != object_count) && object_count > 0) {
		instance_buffer = resource_manager->CreateBuffer(object_count, sizeof(SceneLBVHInstance), true);
		resource_manager->ResourceBarrier(instance_buffer, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
	}
	if (objects_changed && object_count > 0)
		instance_buffer->Update(mesh_instances.data(), object_count);


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

	offsets_updated = vertex_buffers_changed || mesh_buffers_changed;
	vertex_buffers_changed = mesh_buffers_changed = objects_changed = false;
}