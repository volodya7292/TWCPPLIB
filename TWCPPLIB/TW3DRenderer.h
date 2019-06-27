#pragma once
#include "TW3DSwapChain.h"
#include "TW3DSCFrame.h"
#include "TW3DScene.h"

class TW3DRenderer {
public:
	TW3DRenderer() = default;
	virtual ~TW3DRenderer() = 0;
	virtual void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height);
	virtual void InitializeFrame(TW3DSCFrame* Frame);
	virtual void Resize(TWT::uint Width, TWT::uint Height);
	virtual void Record(TW3DSCFrame* Frame) = 0;
	// Per frame
	virtual void Update(float DeltaTime) = 0;
	virtual void Execute(TW3DSCFrame* Frame) = 0;

	TW3DScene* Scene = nullptr;

protected:
	bool Initialized = false;
	TW3DDevice* Device = nullptr;
	TW3DResourceManager* ResourceManager = nullptr;
	TW3DSwapChain* SwapChain = nullptr;
	TWT::uint Width = 0;
	TWT::uint Height = 0;
};
