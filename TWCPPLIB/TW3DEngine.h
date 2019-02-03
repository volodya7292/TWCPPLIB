#pragma once
#include "TWUtils.h"

namespace TW3D {
	struct InitializeInfo {
		TWT::UInt		width;
		TWT::UInt		height;
		TWT::String		title;
		TWT::Bool		fullscreen;
		TWT::Bool		vsync;
	};

	void Start(const InitializeInfo& info);
} // TW3D