#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DCube.h"
#include "TW3DDefaultRenderer.h"

static const float movement_speed = 2.0f;
static const float mouseSensitivity = 0.05f;

static bool movement_capture = true;

TW3DDefaultRenderer* defaultRenderer;
TW3DScene* scene;
TW3DLightSource *light, *light2;

TW3DCube* cube, *cube2;

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
		TWT::vec2i centerMousePos = TW3D::GetWindowCenterPosition();

		TWT::vec2i mousePos = TW3D::GetCursorPosition();

		float xOffset = (mousePos.x - centerMousePos.x) * mouseSensitivity;
		float yOffset = (centerMousePos.y - mousePos.y) * mouseSensitivity;

		TWT::vec3 rotation = scene->Camera->GetRotation();
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


	//cube2->VMInstance.Transform.SetPosition(TWT::vec3(0.0f, 5, 0));
	//cube->VMInstance.Transform.AdjustRotation(TWT::vec3(0.01f));
	//cube2->VMInstances[0].Transform.SetPosition(TWT::vec3(0.8f, 0, 0));
	//cube2->VMInstances[0].Transform.AdjustRotation(TWT::vec3(0.02f));
	scene->Camera->UpdateConstantBuffer();
}

TWT::uint on_thread_tick(TWT::uint ThreadID, TWT::uint ThreadCount) {
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

void on_key(TWT::uint KeyCode, TW3D::KeyActionType Type) {
	float delta_time = TW3D::GetDeltaTime();

	switch (KeyCode) {
	case VK_ESCAPE:
		if (Type == TW3D::KEY_ACTION_DOWN)
			TW3D::Shutdown();
		break;
	case VK_F11:
		if (Type == TW3D::KEY_ACTION_DOWN)
			TW3D::SetFullScreen(!TW3D::GetFullScreen());
		break;
	case 'T':
		if (Type == TW3D::KEY_ACTION_DOWN)
			movement_capture = !movement_capture;
		break;
	}
}

void on_char(TWT::wchar Symbol) {
	//TWU::CPrintln(Symbol);
}

#include "TW3DPrimitives.h"
int main() {
	TW3D::InitializeInfo info = {};
	info.AdditionalThreadCount = 1;
	info.LogFilename = "Log.log"s;
	TW3D::Initialize(info);

	TW3D::SetVSync(true);

	TW3DResourceManager* RM = TW3D::GetResourceManager();

	scene = new TW3DScene(RM);
	cube = new TW3DCube(RM);
	cube2 = new TW3DCube(RM);

	rp3d::BoxShape shape = rp3d::BoxShape(rp3d::Vector3(0.5, 0.5, 0.5));

	cube2->VMInstance.VertexMesh = TW3DPrimitives::GetPyramid4VertexMesh();
	cube2->VMInstance.Transform.SetPosition(TWT::vec3(0.0f, 5, 0));
	scene->AddObject(cube2);
	cube2->VMInstance.RigidBody->setType(rp3d::BodyType::DYNAMIC);
	cube2->VMInstance.RigidBody->enableGravity(true);
	cube2->VMInstance.RigidBody->addCollisionShape(&shape, rp3d::Transform::identity(), 1);

	cube->VMInstance.Transform.SetPosition(TWT::vec3(0.0f, 0, 0));
	scene->AddObject(cube);
	cube->VMInstance.RigidBody->setType(rp3d::BodyType::STATIC);
	cube->VMInstance.RigidBody->enableGravity(false);
	cube->VMInstance.RigidBody->addCollisionShape(&shape, rp3d::Transform::identity(), 1);
	cube->VMInstance.RigidBody->setTransform(TW3DScene::PhysicalTransform(cube->VMInstance.Transform));

	scene->Camera->FOVY = 45;
	scene->Camera->Position.z = 3;

	light = new TW3DLightSource();
	//light->SetSphereRadius(2);
	//light->SetPosition(TWT::vec3(5, 8, 5));
	light->SetTriangleId(8, cube->VMInstance.VertexMesh->VertexBuffers[0]);

	light2 = new TW3DLightSource();
	//light2->SetSphereRadius(2);
	light2->SetTriangleId(9, cube->VMInstance.VertexMesh->VertexBuffers[0]);
	//light2->SetPosition(TWT::vec3(5, -10, 5));

	scene->AddLightSource(light);
	scene->AddLightSource(light2);

	defaultRenderer = new TW3DDefaultRenderer();
	defaultRenderer->Scene = scene;

	TW3D::SetRenderer(defaultRenderer);

	TW3D::SetOnUpdateEvent(on_update);
	TW3D::SetOnThreadTickEvent(on_thread_tick);
	TW3D::SetOnCleanupEvent(on_cleanup);
	TW3D::SetOnKeyEvent(on_key);
	TW3D::SetOnCharEvent(on_char);

	TW3D::Start();

	return 0;
}