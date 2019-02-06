#pragma once
#include "TW3DDevice.h"
#include "TW3DGraphicsCommandList.h"

namespace TW3D {
	class TW3DCommandQueue {
	public:
		TW3DCommandQueue(TW3DDevice* device, D3D12_COMMAND_LIST_TYPE type);
		~TW3DCommandQueue();

		ID3D12CommandQueue* Get();

		void ExecuteCommandList(TW3DGraphicsCommandList* commandList);
		void ExecuteCommandLists(TWT::Vector<TW3DGraphicsCommandList*> commandLists);

		void SignalFence(ID3D12Fence1* fence, TWT::UInt64 value);

		static TW3DCommandQueue* CreateDirect(TW3DDevice* device);

	private:
		ID3D12CommandQueue* commandQueue;
	};
}

