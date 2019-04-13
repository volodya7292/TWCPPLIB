#pragma once
#include "TW3DUtils.h"
#include "TW3DScene.h"
#include "TW3DRenderer.h"
#include "TW3DResourceManager.h"

namespace TW3D {
	struct InitializeInfo {
		TWT::UInt Width;
		TWT::UInt Height;
		TWT::String	Title;
		TWT::Bool FullScreen;
		TWT::Bool VSync;
	};

	void Initialize(const InitializeInfo& info);
	void Start();
	void SetScene(TW3DScene* Scene);
	void SetRenderer(TW3DRenderer* Renderer);

	void SetOnUpdateEvent(void (*OnUpdate)());

	TW3DResourceManager* GetResourceManager();
}