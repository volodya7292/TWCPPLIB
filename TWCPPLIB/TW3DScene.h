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
		TWT::Vector<TW3DObject*> Objects;

		TW3DResourceUAV* gvb;
		TWT::Vector<std::pair<TW3DVertexMesh*, std::pair<TWT::UInt, TWT::UInt>>>   vertex_meshes;

	private:
		TW3DResourceManager* resource_manager;

		TWT::Vector<std::pair<TW3DVertexBuffer*, TWT::UInt>> vertex_buffers;
		

		
		TWT::UInt gvb_vertex_count;
		TW3DResourceUAV* gnb;
		TWT::UInt gnb_node_count;
	};
}
