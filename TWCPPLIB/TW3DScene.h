#pragma once
#include "TW3DObject.h"
#include "TW3DPerspectiveCamera.h"

namespace TW3D {
	class TW3DScene {
	public:
		TW3DScene(TW3DResourceManager* ResourceManager);
		~TW3DScene();

		void AddObject(TW3DObject* Object);

		TW3DPerspectiveCamera* Camera;
		std::unordered_set<TW3DObject*> objects;
	};
}
