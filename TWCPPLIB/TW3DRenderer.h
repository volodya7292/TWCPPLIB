#pragma once
#include "TW3DSwapChain.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DScene.h"

class TW3DRenderer {
public:
	TW3DRenderer() = default;
	virtual ~TW3DRenderer();
	virtual void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
	virtual void Resize(TWT::UInt Width, TWT::UInt Height);
	virtual void Record(TWT::UInt BackBufferIndex, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStesncilOutput);
	virtual void RecordBeforeExecution() = 0;
	void AdjustRecordIndex();
	// Per frame
	virtual void Update(TWT::Float DeltaTime) = 0;
	virtual void Execute(TWT::UInt BackBufferIndex);

	void SetScene(TW3DScene* Scene);

protected:
	TWT::Bool Initialized = false;
	TW3DDevice* Device = nullptr;
	TW3DResourceManager* ResourceManager = nullptr;
	TW3DSwapChain* SwapChain = nullptr;
	TWT::UInt Width = 0;
	TWT::UInt Height = 0;

	TW3DScene* Scene = nullptr;

	TWT::Vector<TW3DGraphicsCommandList*> command_lists;
	TW3DGraphicsCommandList *record_cl = nullptr, *execute_cl = nullptr;
	TWT::UInt current_record_index = 0;
};
