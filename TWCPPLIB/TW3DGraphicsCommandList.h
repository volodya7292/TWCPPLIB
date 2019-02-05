#pragma once
#include "TW3DDevice.h"

namespace TW3D {
	class TW3DGraphicsCommandList {
	public:
		TW3DGraphicsCommandList(TW3D::TW3DDevice device);
		~TW3DGraphicsCommandList();

	private:
		ID3D12GraphicsCommandList* commandList;

	};
}