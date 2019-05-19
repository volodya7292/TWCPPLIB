#pragma once
#include "TW3DSwapChain.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DScene.h"

class TW3DRenderer {
public:
	TW3DRenderer() = default;
	virtual ~TW3DRenderer();
	virtual void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::uint Width, TWT::uint Height);
	virtual void Resize(TWT::uint Width, TWT::uint Height);
	virtual void Record(TWT::uint BackBufferIndex, TW3DRenderTarget* ColorOutput, TW3DResourceDSV* DepthStesncilOutput);
	virtual void RecordBeforeExecution() = 0;
	void AdjustRecordIndex();
	// Per frame
	virtual void Update(float DeltaTime) = 0;
	virtual void Execute(TWT::uint BackBufferIndex);

	void SetScene(TW3DScene* Scene);

protected:
	bool Initialized = false;
	TW3DDevice* Device = nullptr;
	TW3DResourceManager* ResourceManager = nullptr;
	TW3DSwapChain* SwapChain = nullptr;
	TWT::uint Width = 0;
	TWT::uint Height = 0;

	TW3DScene* Scene = nullptr;

	std::vector<TW3DGraphicsCommandList*> command_lists;
	TW3DGraphicsCommandList *record_cl = nullptr, *execute_cl = nullptr;
	TWT::uint current_record_index = 0;
};
