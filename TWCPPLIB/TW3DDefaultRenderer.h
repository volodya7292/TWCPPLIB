#pragma once
#include "TW3DRenderer.h"

namespace TW3D {
	class TW3DDefaultRenderer : public TW3DRenderer {
	public:
		TW3DDefaultRenderer();
		virtual ~TW3DDefaultRenderer();

		void UpdateCommandList(TW3DGraphicsCommandList* CommandList, TW3DScene* Scene);
	};
}