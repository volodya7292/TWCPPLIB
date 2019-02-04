#include "pch.h"
#include "TW3DEngine.h"

int main() {
	TW3D::InitializeInfo info;
	info.width = 1280;
	info.height = 720;
	info.title = "GOVNO!";
	info.fullscreen = false;
	info.vsync = true;
	TW3D::Start(info);
}