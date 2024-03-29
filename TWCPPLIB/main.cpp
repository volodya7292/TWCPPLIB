#include "pch.h"
#include "TW3DEngine.h"
#include "TW3DCube.h"
#include "TW3DDefaultRenderer.h"
#include "TW3DPrimitives.h"

using namespace TWT;


static const float movement_speed = 2.0f;
static const float mouseSensitivity = 0.05f;

static bool movement_capture = true;

TW3DDefaultRenderer* defaultRenderer;
TW3DScene *scene, *large_scene;
TW3DLightSource light, light2;

TW3DCube* cube[100];//, *cube2;

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
		TWT::int2 centerMousePos = TW3D::GetWindowCenterPosition();

		TWT::int2 mousePos = TW3D::GetCursorPosition();

		float xOffset = (mousePos.x - centerMousePos.x) * mouseSensitivity;
		float yOffset = (centerMousePos.y - mousePos.y) * mouseSensitivity;

		TWT::float3 rotation = scene->Camera->GetRotation();
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


	//cube2->VMInstance.Transform.SetPosition(TWT::float3(0.0f, 5, 0));
	//cube->VMInstance.Transform.AdjustRotation(TWT::float3(0.01f));
	
	//cube2->VMInstances[0].Transform.SetPosition(TWT::float3(0.8f, 0, 0));
	//cube2->VMInstances[0].Transform.AdjustRotation(TWT::float3(0.02f));
	scene->Camera->SetSceneScale(1);
	large_scene->Camera->LoadData(scene->Camera);

	scene->Camera->UpdateConstantBuffer();
	large_scene->Camera->UpdateConstantBuffer();
}

TWT::uint on_thread_tick(TWT::uint ThreadID, TWT::uint ThreadCount) {
	TWT::String str = "govno FPS: "s + TW3D::GetFPS();
	TW3D::SetWindowTitle(str);



	return 300;
}

void on_cleanup() {
	for (int i = 0; i < 100; i++)
		delete cube[i];
	//delete cube2;
	delete scene;
	delete large_scene;
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
	case 'H':
		if (Type == TW3D::KEY_ACTION_DOWN)
			light.SetSpherePosition(scene->Camera->Position);
		break;
	}
}

void on_char(TWT::wchar Symbol) {
	//TWU::CPrintln(Symbol);
}

int main() {
	SetConsoleOutputCP(CP_UTF8);

	TW3D::InitializeInfo info = {};
	info.AdditionalThreadCount = 1;
	info.LogFilename = "Log.log"s;
	TW3D::Initialize(info);

	TW3D::SetVSync(true);

	TW3DResourceManager* RM = TW3D::GetResourceManager();

	scene = new TW3DScene(RM);
	large_scene = new TW3DScene(RM);
	large_scene->Camera->SetSceneScale(1);
	//cube = new TW3DCube(RM);
	//cube2 = new TW3DCube(RM);

	rp3d::BoxShape shape = rp3d::BoxShape(rp3d::Vector3(0.5, 0.5, 0.5));

	/*cube2->VMInstance.VertexMesh = TW3DPrimitives::GetPyramid4VertexMesh();
	cube2->VMInstance.Transform.SetPosition(TWT::float3(0.0f, 1, 0));
	cube2->VMInstance.Transform.SetRotation(TWT::double3(0, 0, 180));
	scene->AddObject(cube2);
	cube2->VMInstance.RigidBody->setType(rp3d::BodyType::STATIC);
	cube2->VMInstance.RigidBody->enableGravity(false);
	cube2->VMInstance.RigidBody->addCollisionShape(&shape, rp3d::Transform::identity(), 1);*/



	for (int i = 0; i < 100; i++) {
		cube[i] = new TW3DCube(RM);
		if (i < 1)
			cube[i]->VMInstance.Transform.SetPosition(TWT::float3(0));
		else
			cube[i]->VMInstance.Transform.SetPosition(-50.0f + TWT::float3(rand() % 100, rand() % 100, rand() % 100));

		scene->AddObject(cube[i]);
	}

	


	/*cube->VMInstance.Transform.SetPosition(TWT::float3(0.0f, 0, 0));
	cube->VMInstance.CreateRigidBody = true;
	scene->AddObject(cube);
	cube->VMInstance.RigidBody->setType(rp3d::BodyType::STATIC);
	cube->VMInstance.RigidBody->enableGravity(false);
	cube->VMInstance.RigidBody->addCollisionShape(&shape, rp3d::Transform::identity(), 1);*/
	//cube->VMInstance.RigidBody->setTransform(PhysicalTransform(cube->VMInstance.Transform));
	//cube->VMInstance.Transform.SetIdentity();

	scene->Camera->FOVY = 45;
	//scene->Camera->SetRotation(TWT::float3(0, -90, 0))
	//scene->Camera.
	//scene->Camera->Position.z = 3;

	//light.SetTriangleId(8, cube->VMInstance.VertexMesh->VertexBuffers[0]);
	light.SetSphereRadius(0);
	//light.SetPosition(TWT::float3(0, 50, 0));
	//light.SetPosition(TWT::float3(0, -3, 0));

	//light2.SetTriangleId(9, cube->VMInstance.VertexMesh->VertexBuffers[0]);

	//light2.SetSphereRadius(0.1);
	//light2.SetPosition(TWT::float3(0, -10, 0));

	scene->AddLightSource(&light);
	//scene->AddLightSource(&light2);

	defaultRenderer = new TW3DDefaultRenderer();
	defaultRenderer->Scene = scene;
	//defaultRenderer->LargeScaleScene = large_scene;

	TW3D::SetRenderer(defaultRenderer);

	TW3D::SetOnUpdateEvent(on_update);
	TW3D::SetOnThreadTickEvent(on_thread_tick);
	TW3D::SetOnCleanupEvent(on_cleanup);
	TW3D::SetOnKeyEvent(on_key);
	TW3D::SetOnCharEvent(on_char);

	TW3D::Start();

	return 0;
}