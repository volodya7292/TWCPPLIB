#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"

struct TW3DSceneVertexMesh {
	TWT::uint gvb_offset = -1;
	TWT::uint gnb_offset = -1;
};

class TW3DScene {
public:
	TW3DScene(TW3DResourceManager* ResourceManager);
	~TW3DScene();

	void Bind(TW3DGraphicsCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI);
	void AddObject(TW3DObject* Object);
	void Update(float DeltaTime);
	void RecordBeforeExecution();

	TW3DPerspectiveCamera* Camera;
	std::vector<TW3DObject*> Objects;

private:
	rp3d::Transform PhysicalTransform(TW3DTransform Transform);

	TW3DResourceManager* resource_manager;

	bool vertex_buffers_changed = false;
	bool mesh_buffers_changed = false;
	bool objects_changed = false;

	std::map<TW3DVertexBuffer*, TWT::uint> vertex_buffers;
	std::map<TW3DVertexMesh*, TW3DSceneVertexMesh> vertex_meshes;
	std::vector<SceneLBVHInstance> mesh_instances;

	TW3DBuffer* gvb;
	TW3DBuffer* gnb;
	TW3DBuffer* instance_buffer = nullptr;

	TWT::uint gvb_vertex_count = 0;
	TWT::uint gnb_node_count = 0;

	TW3DLBVH* LBVH;

	rp3d::DynamicsWorld* collision_world;
};