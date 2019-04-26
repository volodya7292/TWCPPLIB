#pragma once
#include "TW3DSwapChain.h"
#include "TW3DGraphicsCommandList.h"
#include "TW3DScene.h"

namespace TW3D {
	class TW3DRenderer {
	public:
		TW3DRenderer() = default;
		virtual ~TW3DRenderer();
		virtual void Initialize(TW3DResourceManager* ResourceManager, TW3DSwapChain* SwapChain, TWT::UInt Width, TWT::UInt Height);
		virtual void Resize(TWT::UInt Width, TWT::UInt Height);
		virtual void Record(TWT::UInt BackBufferIndex, TW3DResourceRTV* ColorOutput, TW3DResourceDSV* DepthStesncilOutput);
		virtual void RecordBeforeExecution();
		void AdjustRecordIndex();
		// Per frame
		virtual void Update();
		virtual void Execute(TWT::UInt BackBufferIndex);

		void SetScene(TW3DScene* Scene);

	protected:
		TWT::Bool Initialized = false;
		TW3DDevice* Device = nullptr;
		TW3DResourceManager* ResourceManager = nullptr;
		TW3DSwapChain* SwapChain = nullptr;
		TWT::UInt Width = 0;
		TWT::UInt Height = 0;

		TW3D::TW3DScene* Scene;

		TWT::Vector<TW3D::TW3DGraphicsCommandList*> command_lists;
		TW3DGraphicsCommandList *record_cl, *execute_cl;
		TWT::UInt current_record_index = 0;
	};
}

