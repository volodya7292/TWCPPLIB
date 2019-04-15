#include "pch.h"
#include "TW3DCommandQueue.h"

TW3D::TW3DCommandQueue::TW3DCommandQueue(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type = Type;

	Device->CreateCommandQueue(&desc, &command_queue);
	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, &fence);
	command_queue->SetName(L"TW3DCommandQueue");
	fence->SetName(L"ID3D12Fence from TW3DCommandQueue");
}

TW3D::TW3DCommandQueue::~TW3DCommandQueue() {
	TWU::DXSafeRelease(command_queue);
	TWU::DXSafeRelease(fence);
}

ID3D12CommandQueue* TW3D::TW3DCommandQueue::Get() {
	return command_queue;
}

void TW3D::TW3DCommandQueue::FlushCommands() {
	TWU::SuccessAssert(command_queue->Signal(fence, ++fence_flush_value));

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (fence->GetCompletedValue() < fence_flush_value) {
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		HANDLE fenceEvent = NULL;
		TWU::SuccessAssert(fence->SetEventOnCompletion(fence_flush_value, fenceEvent));

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void TW3D::TW3DCommandQueue::ExecuteCommandList(TW3DGraphicsCommandList* CommandList) {
	FlushCommands();
	ID3D12CommandList* native_list = CommandList->Get();
	command_queue->ExecuteCommandLists(1, &native_list);
}

void TW3D::TW3DCommandQueue::ExecuteCommandLists(const TWT::Vector<TW3DGraphicsCommandList*>& ÑommandLists) {
	FlushCommands();

	TWT::Vector<ID3D12CommandList*> nativeLists(ÑommandLists.size());
	for (TWT::UInt i = 0; i < nativeLists.size(); i++)
		nativeLists[i] = ÑommandLists[i]->Get();

	command_queue->ExecuteCommandLists(static_cast<UINT>(nativeLists.size()), nativeLists.data());
}

TW3D::TW3DCommandQueue* TW3D::TW3DCommandQueue::CreateDirect(TW3DDevice* Âevice) {
	return new TW3DCommandQueue(Âevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3D::TW3DCommandQueue* TW3D::TW3DCommandQueue::CreateCompute(TW3DDevice* Âevice) {
	return new TW3DCommandQueue(Âevice, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}
