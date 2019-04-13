#include "pch.h"
#include "TW3DRenderer.h"

void TW3D::TW3DRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height) {
	this->ResourceManager = ResourceManager;
	this->SwapChain = SwapChain;
	this->Width = Width;
	this->Height = Height;
	Initialized = true;
}

void TW3D::TW3DRenderer::Resize(TWT::UInt Width, TWT::UInt Height) {
	this->Width = Width;
	this->Height = Height;
}

void TW3D::TW3DRenderer::Record(TW3DScene* Scene, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStencilOutput) {
}

void TW3D::TW3DRenderer::Execute() {
}
