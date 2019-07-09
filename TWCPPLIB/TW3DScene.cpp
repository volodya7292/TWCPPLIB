#include "pch.h"
#include "TW3DScene.h"
#include "TW3DShaders.h"

TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager) {
	Camera = new TW3DPerspectiveCamera(ResourceManager);

	lsb = ResourceManager->CreateBuffer("lsb", 1024, sizeof(TWT::DefaultLightSource), false, true, D3D12_RESOURCE_STATE_COPY_DEST);

	obj_cmds.resize(16384);

	obj_cbs = ResourceManager->CreateConstantBuffer("objs_cb_buffer"s, obj_cmds.size(), sizeof(TWT::DefaultModelCB));

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
	delete obj_cbs;
	delete collision_world;
}

std::vector<TW3DObject*> const& TW3DScene::GetObjects() const {
	return objects;
}

std::vector<TW3DLightSource*> const& TW3DScene::GetLightSources() const {
	return light_sources;
}

std::vector<TW3DScene::ObjectCmd> const& TW3DScene::GetObjectRenderCommands() const {
	return obj_cmds;
}

const TWT::uint TW3DScene::GetObjectRenderCommandCount() const {
	return obj_cmd_count;
}

void TW3DScene::Bind(TW3DCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI, TWT::uint LSBRPI) {
	CommandList->BindBuffer(LSBRPI, lsb);
}

void TW3DScene::AddObject(TW3DObject* Object) {
	objects.push_back(Object);
	objects_changed = true;

	TWT::uint cmd_index = obj_cmd_count;

	for (auto& vminst : Object->GetVertexMeshInstances()) {
		mesh_inst_cmd_indices[&vminst] = cmd_index;

		obj_cmds[cmd_index].cb = obj_cbs->GetGPUVirtualAddress(cmd_index);
		obj_cmds[cmd_index].vb = vminst.VertexBuffer->GetView();
		obj_cmds[cmd_index].draw = { vminst.VertexBuffer->GetVertexCount(), 1, 0, 0};

		if (vminst.CreateRigidBody && !vminst.RigidBody)
			vminst.RigidBody = collision_world->createRigidBody(vminst.Transform.GetPhysicalTransform());
	}
}

void TW3DScene::AddLightSource(TW3DLightSource* LightSource) {
	light_sources.push_back(LightSource);
}

void TW3DScene::Update(float DeltaTime) {
	for (TW3DObject* object : objects)
		object->Update();

	for (TWT::uint i = 0; i < light_sources.size(); i++) {
		TW3DLightSource* light = light_sources[i];

		if (light->Updated) {
			light->Updated = false;
			lsb->UpdateElement(&light->MakeInfo(), i);
		}
	}
	
	collision_world->update(DeltaTime);
}