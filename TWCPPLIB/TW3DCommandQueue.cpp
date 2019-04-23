#include "pch.h"
#include "TW3DCommandQueue.h"

TW3D::TW3DCommandQueue::TW3DCommandQueue(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type = Type;

	Device->CreateCommandQueue(&desc, &command_queue);
	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, &fence);
	fence->SetName(L"ID3D12Fence from TW3DCommandQueue");

	if (Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		command_queue->SetName(L"TW3DCommandQueue Compute");
	else if (Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
		command_queue->SetName(L"TW3DCommandQueue Direct");

}

TW3D::TW3DCommandQueue::~TW3DCommandQueue() {
	TWU::DXSafeRelease(command_queue);
	TWU::DXSafeRelease(fence);
}

ID3D12CommandQueue* TW3D::TW3DCommandQueue::Get() {
	return command_queue;
}

TWT::Bool TW3D::TW3DCommandQueue::IsCommandListRunning(TW3DGraphicsCommandList* CommandList) {
	return fence->GetCompletedValue() < CommandList->SignalValue;
}

void TW3D::TW3DCommandQueue::FlushCommandList(TW3DGraphicsCommandList* CommandList) {
	if (fence->GetCompletedValue() < fence_flush_value) {
		HANDLE fenceEvent = NULL;
		TWU::SuccessAssert(fence->SetEventOnCompletion(CommandList->SignalValue, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void TW3D::TW3DCommandQueue::FlushCommands() {
	TWU::SuccessAssert(command_queue->Signal(fence, ++fence_flush_value));
	if (fence->GetCompletedValue() < fence_flush_value) {
		HANDLE fenceEvent = NULL;
		TWU::SuccessAssert(fence->SetEventOnCompletion(fence_flush_value, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

void TW3D::TW3DCommandQueue::ExecuteCommandList(TW3DGraphicsCommandList* CommandList) {
	FlushCommands();
	CommandList->SignalValue = ++fence_flush_value;

	ID3D12CommandList* native_list = CommandList->Get();
	command_queue->ExecuteCommandLists(1, &native_list);
	TWU::SuccessAssert(command_queue->Signal(fence, fence_flush_value));
}

void TW3D::TW3DCommandQueue::ExecuteCommandLists(const TWT::Vector<TW3DGraphicsCommandList*>& CommandLists) {
	FlushCommands();
	fence_flush_value++;

	TWT::Vector<ID3D12CommandList*> nativeLists(CommandLists.size());
	for (TWT::UInt i = 0; i < nativeLists.size(); i++) {
		nativeLists[i] = CommandLists[i]->Get();
		CommandLists[i]->SignalValue = fence_flush_value;
	}

	command_queue->ExecuteCommandLists(static_cast<UINT>(nativeLists.size()), nativeLists.data());
	TWU::SuccessAssert(command_queue->Signal(fence, fence_flush_value));
}

TW3D::TW3DCommandQueue* TW3D::TW3DCommandQueue::CreateDirect(TW3DDevice* Âevice) {
	return new TW3DCommandQueue(Âevice, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3D::TW3DCommandQueue* TW3D::TW3DCommandQueue::CreateCompute(TW3DDevice* Âevice) {
	return new TW3DCommandQueue(Âevice, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}
