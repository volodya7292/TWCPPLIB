#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DCube.h"

TW3D::TW3DScene* scene;
TW3D::TW3DCube* cube;

int main() {
	TW3D::InitializeInfo info;
	info.width = 1280;
	info.height = 720;
	info.title = "GOVNO!";
	info.fullscreen = false;
	info.vsync = true;
	TW3D::Initialize(info);

	scene = new TW3D::TW3DScene();
	

	TW3D::Start();
}