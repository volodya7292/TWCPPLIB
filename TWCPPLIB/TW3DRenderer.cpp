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
	Scene->Camera->Width = Width;
	Scene->Camera->Height = Height;
}

void TW3D::TW3DRenderer::Record(const TWT::Vector<TW3DResourceRTV*>& ColorOutputs, TW3DResourceDSV* DepthStencilOutput) {
}

void TW3D::TW3DRenderer::Update() {
}

void TW3D::TW3DRenderer::Execute(TWT::UInt BackBufferIndex) {
}

void TW3D::TW3DRenderer::SetScene(TW3DScene* Scene) {
	this->Scene = Scene;
}
