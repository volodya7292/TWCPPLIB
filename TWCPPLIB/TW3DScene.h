#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"

namespace TW3D {
	class TW3DScene {
	public:
		TW3DScene(TW3DResourceManager* ResourceManager);
		~TW3DScene();

		void AddObject(TW3DObject* Object);
		void RecordBeforeExecution();

		TW3DPerspectiveCamera* Camera;
		TWT::Vector<TW3DObject*> objects;

	private:
		TW3DResourceManager* resource_manager;

		TWT::Vector<std::pair<TW3DVertexBuffer*, TWT::UInt>> gvb_vertex_buffers;
		TWT::Vector<std::pair<TW3DVertexMesh*, TWT::UInt>>   gvb_vertex_meshes;

		TW3DResourceUAV* gvb;
		TWT::UInt gvb_vertex_count;
		TW3DResourceUAV* gnb;
		TWT::UInt gnb_node_count;
	};
}
