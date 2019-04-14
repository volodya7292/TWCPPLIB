#include "pch.h"
#include "TW3DScene.h"

TW3D::TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) {
	Camera = new TW3DPerspectiveCamera(ResourceManager);
}

TW3D::TW3DScene::~TW3DScene() {
	delete Camera;
}

void TW3D::TW3DScene::AddObject(TW3DObject* Object) {
	objects.insert(Object);
}