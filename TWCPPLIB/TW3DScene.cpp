#include "pch.h"
#include "TW3DScene.h"
#include "TW3DShaders.h"

TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager) {
	Camera = new TW3DPerspectiveCamera(ResourceManager);

	lsb = ResourceManager->CreateBuffer(1024, sizeof(TW3DSceneLightSource), true);
	ResourceManager->ResourceBarrier(lsb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST);

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
	delete lsb;
	delete collision_world;
}

void TW3DScene::Bind(TW3DCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI, TWT::uint LSBRPI) {
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

}