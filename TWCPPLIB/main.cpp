#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DCube.h"
#include "TW3DDefaultRenderer.h"

TW3D::TW3DScene* scene;
TW3D::TW3DCube* cube;

void on_update() {
	cube->Transform.AdjustRotation(TWT::Vector3f(0.01f));
	scene->Camera->UpdateConstantBuffer();
}

int main() {
	TW3D::InitializeInfo info;
	info.Width = 1280;
	info.Height = 720;
	info.Title = "GOVNO!";
	info.FullScreen = false;
	info.VSync = true;
	TW3D::Initialize(info);

	TW3D::TW3DResourceManager* RM = TW3D::GetResourceManager();

	scene = new TW3D::TW3DScene(RM);
	cube = new TW3D::TW3DCube(RM);
	scene->AddObject(cube);
	
	scene->Camera->FOVY = 45;
	scene->Camera->Position.z = 3;

	TW3D::TW3DDefaultRenderer* defaultRenderer = new TW3D::TW3DDefaultRenderer();
	TW3D::SetRenderer(defaultRenderer);
	TW3D::SetScene(scene);

	TW3D::SetOnUpdateEvent(on_update);

	TW3D::Start();
}