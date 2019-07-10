#include "pch.h"
#include "TW3DRenderer.h"

TW3DRenderer::~TW3DRenderer() {
	
}

void TW3DRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint2 Size) {
	this->ResourceManager = ResourceManager;
	this->SwapChain = SwapChain;
	this->Size = Size;
	Initialized = true;
	Device = ResourceManager->GetDevice();
}

void TW3DRenderer::Resize(TWT::uint2 Size) {
	this->Size = Size;
	Scene->Camera->Width = Size.x;
	Scene->Camera->Height = Size.y;
}

void TW3DRenderer::Execute(TW3DSCFrame* Frame) {
	Frame->PerformSwap();
}
