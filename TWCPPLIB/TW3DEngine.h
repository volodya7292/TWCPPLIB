#pragma once
#include "TW3DUtils.h"
#include "TW3DScene.h"
#include "TW3DRenderingMethod.h"

namespace TW3D {
	struct InitializeInfo {
		TWT::UInt		width;
		TWT::UInt		height;
		TWT::String		title;
		TWT::Bool		fullscreen;
		TWT::Bool		vsync;
	};

	void Start(const InitializeInfo& info);
	void SetScene(TW3DScene* Scene);
	void SetRenderingMethod(TW3DRenderingMethod* RenderingMethod);
}