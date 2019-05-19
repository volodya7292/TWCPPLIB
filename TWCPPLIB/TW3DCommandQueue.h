#pragma once
#include "TW3DDevice.h"
#include "TW3DGraphicsCommandList.h"

class TW3DCommandQueue {
public:
	TW3DCommandQueue(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type);
	~TW3DCommandQueue();

	ID3D12CommandQueue* Get();

	bool IsCommandListRunning(TW3DGraphicsCommandList* CommandList);
	void FlushCommandList(TW3DGraphicsCommandList* CommandList);
	void FlushCommands();
	void ExecuteCommandList(TW3DGraphicsCommandList* CommandList);
	void ExecuteCommandLists(const std::vector<TW3DGraphicsCommandList*>& CommandLists);

	static TW3DCommandQueue* CreateDirect(TW3DDevice* Device);
	static TW3DCommandQueue* CreateCompute(TW3DDevice* Device);
	static TW3DCommandQueue* CreateCopy(TW3DDevice* Device);

private:
	ID3D12CommandQueue* command_queue;
	ID3D12Fence* fence;
	TWT::uint64 fence_flush_value = 0;
};