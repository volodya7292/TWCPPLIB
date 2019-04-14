#pragma once
#include "TW3DUtils.h"
#include "TW3DScene.h"
#include "TW3DRenderer.h"
#include "TW3DResourceManager.h"

namespace TW3D {
	struct InitializeInfo {
		TWT::UInt Width = 1280;
		TWT::UInt Height = 720;
		TWT::String	Title = "TW3DEngine";
		TWT::Bool FullScreen = false;
		TWT::Bool VSync = true;
		TWT::UInt AdditionalThreadCount = 0;
	};

	void Initialize(const InitializeInfo& info);
	void Start();
	void SetRenderer(TW3DRenderer* Renderer);

	void SetOnUpdateEvent(void (*OnUpdate)());
	void SetOnThreadTickEvent(TWT::UInt(*on_thread_tick)(TWT::UInt ThreadID, TWT::UInt ThreadCount));
	void SetOnCleanupEvent(void (*OnCleanup)());

	TW3DResourceManager* GetResourceManager();
}