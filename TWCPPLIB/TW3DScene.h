#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"
#include "TW3DLightSource.h"

class TW3DScene {
public:
	TW3DScene(TW3DResourceManager* ResourceManager);
	~TW3DScene();

	void Bind(TW3DCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI, TWT::uint LSBRPI);
	void AddObject(TW3DObject* Object);
	void AddLightSource(TW3DLightSource* LightSource);
	void Update(float DeltaTime);
	void BeforeExecution();

	TW3DPerspectiveCamera* Camera;
	std::vector<TW3DObject*> Objects;
	std::vector<TW3DLightSource*> LightSources;

private:
	struct TW3DSceneVertexMesh {
		TWT::uint gvb_offset = -1;
		TWT::uint gnb_offset = -1;
	};

	struct TW3DSceneLightSource {
		TWT::float4 position;
		TWT::float4 color;
		TWT::float4 info; // .x - type, .y - triangle id, z - sphere radius
	};

	TW3DResourceManager* resource_manager;

	bool vertex_buffers_changed = false;
	bool mesh_buffers_changed = false;
	bool objects_changed = false;
	bool offsets_updated = false;

	std::map<TW3DVertexBuffer*, TWT::uint> vertex_buffers;
	std::map<TW3DVertexMesh*, TW3DSceneVertexMesh> vertex_meshes;

	TW3DBuffer *lsb;


	rp3d::DynamicsWorld* collision_world;
};