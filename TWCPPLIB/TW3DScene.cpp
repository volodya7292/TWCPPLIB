#include "pch.h"
#include "TW3DScene.h"


TW3D::TW3DScene::TW3DScene() {
}


TW3D::TW3DScene::~TW3DScene() {
}

void TW3D::TW3DScene::AddObject(TW3DObject* Object) {
	objects.insert(Object);
}
