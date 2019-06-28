#include "pch.h"
#include "TW3DRenderer.h"

TW3DRenderer::~TW3DRenderer() {
	
}

void TW3DRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height) {
	this->ResourceManager = ResourceManager;
	this->SwapChain = SwapChain;
	this->Width = Width;
	this->Height = Height;
	Initialized = true;
	Device = ResourceManager->GetDevice();
}

void TW3DRenderer::Resize(TWT::uint Width, TWT::uint Height) {
	this->Width = Width;
	this->Height = Height;
	Scene->Camera->Width = Width;
	Scene->Camera->Height = Height;
}

void TW3DRenderer::Execute(TW3DSCFrame* Frame) {
	Frame->PerformSwap();
}
