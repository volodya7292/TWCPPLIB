#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"
#include "TW3DLightSource.h"

class TW3DScene {
public:
	struct ObjectCmd {
		D3D12_GPU_VIRTUAL_ADDRESS cb;
		D3D12_VERTEX_BUFFER_VIEW  vb;
		D3D12_DRAW_ARGUMENTS      draw;
	};

	TW3DScene(TW3DResourceManager* ResourceManager);
	~TW3DScene();

	std::vector<TW3DObject*> const& GetObjects() const;
	std::vector<TW3DLightSource*> const& GetLightSources() const;
	std::vector<ObjectCmd> const& GetObjectRenderCommands() const;
	const TWT::uint GetObjectRenderCommandCount() const;

	void Bind(TW3DCommandList* CommandList, TWT::uint GVBRPI, TWT::uint SceneRTNBRPI, TWT::uint GNBRPI, TWT::uint LSBRPI);
	void AddObject(TW3DObject* Object);
	void AddLightSource(TW3DLightSource* LightSource);
	void Update(float DeltaTime);

	TW3DPerspectiveCamera* Camera;

private:
	TW3DResourceManager* resource_manager;


	bool objects_changed = false;

	std::vector<TW3DObject*> objects;
	std::unordered_map<TW3DVertexMeshInstance*, TWT::uint> mesh_inst_cmd_indices; // vertex mesh instance -> cmd index

	std::vector<TW3DLightSource*> light_sources;


	TW3DBuffer *lsb;

	TW3DConstantBuffer* obj_cbs;
	std::vector<ObjectCmd> obj_cmds;
	TWT::uint obj_cmd_count = 0;

	rp3d::DynamicsWorld* collision_world;
};