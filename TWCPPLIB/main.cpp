#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DCube.h"
#include "TW3DDefaultRenderer.h"

static const TWT::Float movement_speed = 2.0f;
static const TWT::Float mouseSensitivity = 0.05f;

static TWT::Bool movement_capture = true;

TW3D::TW3DDefaultRenderer* defaultRenderer;
TW3D::TW3DScene* scene;


TW3D::TW3DCube* cube, *cube2;

void on_update() {

	if (movement_capture) {
		int to_back_movement = 0;
		int right_left_movement = 0;
		int up_down_movement = 0;

		if (TW3D::IsKeyDown('W'))
			to_back_movement++;
		if (TW3D::IsKeyDown('S'))
			to_back_movement--;

		if (TW3D::IsKeyDown('A'))
			right_left_movement--;
		if (TW3D::IsKeyDown('D'))
			right_left_movement++;

		if (TW3D::IsKeyDown(VK_SPACE))
			up_down_movement++;
		if (TW3D::IsKeyDown(VK_SHIFT))
			up_down_movement--;

		float ms = movement_speed * TW3D::GetDeltaTime();

		// Move camera in space
		scene->Camera->Position.y += up_down_movement * ms;

		scene->Camera->Move(to_back_movement * ms, right_left_movement * ms);

		// Mouse movement
		TWT::Vector2i centerMousePos = TW3D::GetWindowCenterPosition();

		TWT::Vector2i mousePos = TW3D::GetCursorPosition();

		float xOffset = (mousePos.x - centerMousePos.x) * mouseSensitivity;
		float yOffset = (centerMousePos.y - mousePos.y) * mouseSensitivity;

		TWT::Vector3f rotation = scene->Camera->GetRotation();
		float xRotation = rotation.x;
		float newXRotation = xRotation - yOffset;


		if (newXRotation <= 90.0f && newXRotation >= -90.0f)
			rotation.x = newXRotation;
		else if (newXRotation > 0)
			rotation.x = 90.0f;
		else if (newXRotation < 0)
			rotation.x = -90.0f;

		rotation.y += xOffset;
		scene->Camera->SetRotation(rotation);

		SetCursorPos(centerMousePos.x, centerMousePos.y);
	}



	//cube->VMInstances[0].Transform.SetPosition(TWT::Vector3f(-0.8f, 0, 0));
	//cube->VMInstances[0].Transform.AdjustRotation(TWT::Vector3f(0.01f));
	//cube2->VMInstances[0].Transform.SetPosition(TWT::Vector3f(0.8f, 0, 0));
	//cube2->VMInstances[0].Transform.AdjustRotation(TWT::Vector3f(0.02f));
	scene->Camera->UpdateConstantBuffer();
}

TWT::UInt on_thread_tick(TWT::UInt ThreadID, TWT::UInt ThreadCount) {
	TWT::String str = "govno FPS: "s + TW3D::GetFPS();
	TW3D::SetWindowTitle(str);

	return 300;
}

void on_cleanup() {
	delete cube;
	delete cube2;
	delete scene;
	delete defaultRenderer;
}

void on_key(TWT::UInt KeyCode, TW3D::TW3DKeyActionType Type) {
	float delta_time = TW3D::GetDeltaTime();

	switch (KeyCode) {
	case VK_ESCAPE:
		if (Type == TW3D::TW3D_KEY_ACTION_DOWN)
			TW3D::Shutdown();
		break;
	case VK_F11:
		if (Type == TW3D::TW3D_KEY_ACTION_DOWN)
			TW3D::SetFullScreen(!TW3D::GetFullScreen());
		break;
	case 'T':
		if (Type == TW3D::TW3D_KEY_ACTION_DOWN)
			movement_capture = !movement_capture;
		break;
	}
}

void on_char(TWT::WChar Symbol) {
	//TWU::CPrintln(Symbol);
}

#include "TW3DPrimitives.h"
int main() {
	TW3D::InitializeInfo info = {};
	info.AdditionalThreadCount = 1;
	info.LogFilename = "Log.log"s;
	TW3D::Initialize(info);

	TW3D::SetVSync(true);

	TW3D::TW3DResourceManager* RM = TW3D::GetResourceManager();

	scene = new TW3D::TW3DScene(RM);
	cube = new TW3D::TW3DCube(RM);
	cube2 = new TW3D::TW3DCube(RM);

	cube->VMInstances[0].Transform.SetPosition(TWT::Vector3f(0.0f, 0, 0));
	scene->AddObject(cube);

	cube2->VMInstances[0].VertexMesh = TW3DPrimitives::GetPyramid4VertexMesh();
	cube2->VMInstances[0].Transform.SetPosition(TWT::Vector3f(0.0f, 5, 0));
	scene->AddObject(cube2);


	scene->Camera->FOVY = 45;
	scene->Camera->Position.z = 3;

	defaultRenderer = new TW3D::TW3DDefaultRenderer();
	defaultRenderer->SetScene(scene);

	TW3D::SetRenderer(defaultRenderer);

	TW3D::SetOnUpdateEvent(on_update);
	TW3D::SetOnThreadTickEvent(on_thread_tick);
	TW3D::SetOnCleanupEvent(on_cleanup);
	TW3D::SetOnKeyEvent(on_key);
	TW3D::SetOnCharEvent(on_char);

	TW3D::Start();

	return 0;
}