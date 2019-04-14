#pragma once
#include "TW3DCommandQueue.h"

namespace TW3D {
	class TW3DFence {
	public:
		TW3DFence(TW3DDevice* device);
		~TW3DFence();

		void SetFlushValue(TWT::UInt64 value);
		void Flush(TW3DCommandQueue* commandQueue);

		ID3D12Fence1* Get();

	private:
		ID3D12Fence1* fence;
		TWT::UInt64 flushValue;

		HANDLE fenceEvent = NULL;
	};
}