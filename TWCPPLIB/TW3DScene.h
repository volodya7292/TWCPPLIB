#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"

namespace TW3D {
	class TW3DScene {
	public:
		TW3DScene(TW3DResourceManager* ResourceManager);
		~TW3DScene();

		void Bind(TW3DGraphicsCommandList* CommandList, TWT::UInt GVBRPI, TWT::UInt SceneRTNBRPI, TWT::UInt GNBRPI);
		void AddObject(TW3DObject* Object);
		void RecordBeforeExecution();

		TW3DPerspectiveCamera* Camera;
		TWT::Vector<TW3DObject*> Objects;

	private:
		TW3DResourceManager* resource_manager;

		TWT::Bool vertex_buffers_changed = false;
		TWT::Bool mesh_buffers_changed = false;
		TWT::Bool objects_changed = false;

		std::map<TW3DVertexBuffer*, TWT::UInt> vertex_buffers;
		std::map<TW3DVertexMesh*, std::pair<TWT::UInt, TWT::UInt>> vertex_meshes;
		TWT::Vector<SceneLBVHInstance> mesh_instances;

		TW3DResourceUAV* gvb;
		TW3DResourceUAV* gnb;

		TWT::UInt gvb_vertex_count = 0;
		TWT::UInt gnb_node_count = 0;

		TW3DLBVH* LBVH;
	};
}
