#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"
#include "TW3DLightSource.h"

struct TW3DSceneVertexMesh {
	TWT::uint gvb_offset = -1;
	TWT::uint gnb_offset = -1;
};

struct TW3DSceneLightSource {
	TWT::vec4 position;
	TWT::vec4 color;
	TWT::vec4 info; // .x - type, .y - triangle id, z - sphere radius
};

class TW3DScene {
public:
	TW3DScene(TW3DResourceManager* ResourceManager);
	~TW3DScene();

	void Bind(TW3DGraphicsCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI, TWT::uint LSBRPI);
	void AddObject(TW3DObject* Object);
	void AddLightSource(TW3DLightSource* LightSource);
	void Update(float DeltaTime);
	void RecordBeforeExecution();

	TW3DPerspectiveCamera* Camera;
	std::vector<TW3DObject*> Objects;
	std::vector<TW3DLightSource*> LightSources;

private:
	TW3DResourceManager* resource_manager;

	bool vertex_buffers_changed = false;
	bool mesh_buffers_changed = false;
	bool objects_changed = false;
	bool offsets_updated = false;

	std::map<TW3DVertexBuffer*, TWT::uint> vertex_buffers;
	std::map<TW3DVertexMesh*, TW3DSceneVertexMesh> vertex_meshes;
	std::vector<SceneLBVHInstance> mesh_instances;

	TW3DBuffer *gvb, *gnb, *instance_buffer = nullptr, *lsb;

	TWT::uint gvb_vertex_count = 0;
	TWT::uint gnb_node_count = 0;

	TW3DLBVH* LBVH;

	rp3d::DynamicsWorld* collision_world;
};