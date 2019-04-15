#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DCube.h"
#include "TW3DDefaultRenderer.h"

TW3D::TW3DDefaultRenderer* defaultRenderer;
TW3D::TW3DScene* scene;
TW3D::TW3DCube* cube;

void on_update() {
	cube->Transform.AdjustRotation(TWT::Vector3f(0.01f));
	scene->Camera->UpdateConstantBuffer();
}

void on_cleanup() {
	delete cube;
	delete scene;
	delete defaultRenderer;
}

void on_key_down(TWT::UInt KeyCode) {
	switch (KeyCode) {
	case VK_ESCAPE:
		TW3D::Shutdown();
		break;
	case VK_F11:
		TW3D::SetFullScreen(!TW3D::GetFullScreen());
		break;
	}
}

void on_char(TWT::WChar Symbol) {
	TWU::CPrintln(Symbol);
}

int main() {
	TW3D::InitializeInfo info = {};
	TW3D::Initialize(info);

	TW3D::TW3DResourceManager* RM = TW3D::GetResourceManager();

	scene = new TW3D::TW3DScene(RM);
	cube = new TW3D::TW3DCube(RM);
	scene->AddObject(cube);
	
	scene->Camera->FOVY = 45;
	scene->Camera->Position.z = 3;

	defaultRenderer = new TW3D::TW3DDefaultRenderer();
	defaultRenderer->SetScene(scene);

	TW3D::SetRenderer(defaultRenderer);

	TW3D::SetOnUpdateEvent(on_update);
	TW3D::SetOnCleanupEvent(on_cleanup);
	TW3D::SetOnKeyDownEvent(on_key_down);
	TW3D::SetOnCharEvent(on_char);

	TW3D::Start();
}