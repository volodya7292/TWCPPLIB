#pragma once
#include "TW3DUtils.h"
#include "TW3DScene.h"
#include "TW3DRenderer.h"
#include "TW3DResourceManager.h"

namespace TW3D {
	struct InitializeInfo {
		TWT::UInt		width;
		TWT::UInt		height;
		TWT::String		title;
		TWT::Bool		fullscreen;
		TWT::Bool		vsync;
	};

	void Initialize(const InitializeInfo& info);
	void Start();
	void SetScene(TW3DScene* Scene);
	void SetRenderer(TW3DRenderer* RenderingMethod);

	TW3DResourceManager* GetResourceManager();
}