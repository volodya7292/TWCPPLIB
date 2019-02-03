#include "pch.h"
#include "TW3DEngine.h"

int main()
{
	//
	//// initialize direct3d
	//if (!InitD3D()) {
	//	MessageBox(0, L"Failed to initialize direct3d 12",
	//		L"Error", MB_OK);
	//	Cleanup();
	//	return 1;
	//}



	//// we want to wait for the gpu to finish executing the command list before we start releasing everything
	//WaitForPreviousFrame();

	//// close the fence event
	//CloseHandle(fenceEvent);

	TW3D::InitializeInfo info;
	info.width = 1280;
	info.height = 720;
	info.title = "GOVNO!";
	info.fullscreen = false;
	info.vsync = true;
	TW3D::Start(info);

	//getchar();
}