#include "pch.h"
#include "TW3DRenderer.h"

TW3DRenderer::~TW3DRenderer() {
	for (size_t i = 0; i < TW3DSwapChain::BufferCount * 2; i++)
		delete command_lists[i];
}

void TW3DRenderer::Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height) {
	this->ResourceManager = ResourceManager;
	this->SwapChain = SwapChain;
	this->Width = Width;
	this->Height = Height;
	Initialized = true;
	Device = ResourceManager->GetDevice();

	for (size_t i = 0; i < TW3DSwapChain::BufferCount * 2; i++)
		command_lists.push_back(ResourceManager->CreateDirectCommandList());
}

void TW3DRenderer::Resize(TWT::uint Width, TWT::uint Height) {
	this->Width = Width;
	this->Height = Height;
	Scene->Camera->Width = Width;
	Scene->Camera->Height = Height;

	for (size_t i = 0; i < command_lists.size(); i++)
		command_lists[i]->EmptyReset();
}

void TW3DRenderer::Record(TWT::uint BackBufferIndex, TW3DRenderTarget* ColorOutput, TW3DResourceDSV* DepthStesncilOutput) {
	record_cl = current_record_index == 0 ? command_lists[BackBufferIndex * 2] : command_lists[BackBufferIndex * 2 + 1];
	ResourceManager->FlushCommandList(record_cl);
}

void TW3DRenderer::AdjustRecordIndex() {
	current_record_index = (current_record_index + 1) % 2;
}

void TW3DRenderer::Execute(TWT::uint BackBufferIndex) {
	execute_cl = current_record_index == 0 ? command_lists[BackBufferIndex * 2 + 1] : command_lists[BackBufferIndex * 2];
	while (execute_cl->IsEmpty())
		Sleep(1);
}

void TW3DRenderer::SetScene(TW3DScene* Scene) {
	this->Scene = Scene;
}
