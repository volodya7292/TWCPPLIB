#pragma once
#include "TW3DUtils.h"
#include "TW3DRenderer.h"

namespace TW3D {
	struct InitializeInfo {
		TWT::UInt Width = 1280;
		TWT::UInt Height = 720;
		TWT::String	Title = "TW3DEngine";
		TWT::UInt AdditionalThreadCount = 0;
	};

	void Initialize(const InitializeInfo& info);
	void Start();
	TWT::Bool GetFullScreen();
	void SetFullScreen(TWT::Bool Fullscreen);
	TWT::Bool GetVSync();
	void SetVSync(TWT::Bool VSync);
	void SetRenderer(TW3DRenderer* Renderer);

	void SetOnUpdateEvent(void (*OnUpdate)());
	void SetOnThreadTickEvent(TWT::UInt(*on_thread_tick)(TWT::UInt ThreadID, TWT::UInt ThreadCount));
	void SetOnCleanupEvent(void (*OnCleanup)());

	TW3DResourceManager* GetResourceManager();
}