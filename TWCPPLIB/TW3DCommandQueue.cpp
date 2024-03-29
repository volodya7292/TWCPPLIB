#include "pch.h"
#include "TW3DCommandQueue.h"

TW3DCommandQueue::TW3DCommandQueue(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type = Type;

	Device->CreateCommandQueue(&desc, &command_queue);
	Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, &fence);
	TWU::SuccessAssert(fence->SetName(L"ID3D12Fence from TW3DCommandQueue"), "TW3DCommandQueue::TW3DCommandQueue, fence->SetName");

	if (Type == D3D12_COMMAND_LIST_TYPE_DIRECT)
		TWU::SuccessAssert(command_queue->SetName(L"TW3DCommandQueue Direct"), "TW3DCommandQueue::TW3DCommandQueue, (Direct)command_queue->SetName 1");
	else if (Type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
		TWU::SuccessAssert(command_queue->SetName(L"TW3DCommandQueue Compute"), "TW3DCommandQueue::TW3DCommandQueue, (Compute)command_queue->SetName");
	else if (Type == D3D12_COMMAND_LIST_TYPE_COPY)
		TWU::SuccessAssert(command_queue->SetName(L"TW3DCommandQueue Copy"), "TW3DCommandQueue::TW3DCommandQueue, (Copy)command_queue->SetName 1");

}

TW3DCommandQueue::~TW3DCommandQueue() {
	TWU::DXSafeRelease(command_queue);
	TWU::DXSafeRelease(fence);
}

ID3D12CommandQueue* TW3DCommandQueue::Get() {
	return command_queue;
}

bool TW3DCommandQueue::IsCommandListRunning(TW3DCommandList* CommandList) {
	return fence->GetCompletedValue() < CommandList->SignalValue;
}

void TW3DCommandQueue::FlushCommandList(TW3DCommandList* CommandList) {
	if (fence->GetCompletedValue() < CommandList->SignalValue) {
		HANDLE fence_event = nullptr;
		TWU::SuccessAssert(fence->SetEventOnCompletion(CommandList->SignalValue, fence_event), "TW3DCommandQueue::FlushCommandList "s + CommandList->SignalValue);
		WaitForSingleObject(fence_event, INFINITE);
	}
}

void TW3DCommandQueue::FlushCommands() {
	TWU::SuccessAssert(command_queue->Signal(fence, fence_flush_value), "TW3DCommandQueue::FlushCommands, command_queue->Signal "s + fence_flush_value);

	if (fence->GetCompletedValue() < fence_flush_value) {
		HANDLE fence_event = nullptr;
		TWU::SuccessAssert(fence->SetEventOnCompletion(fence_flush_value, fence_event), "TW3DCommandQueue::FlushCommands, fence->SetEventOnCompletion "s + fence_flush_value);
		WaitForSingleObject(fence_event, INFINITE);
	}
}

void TW3DCommandQueue::ExecuteCommandList(TW3DCommandList* CommandList) {
	if (CommandList->IsEmpty())
		return;

	CommandList->SignalValue = ++fence_flush_value;

	ID3D12CommandList* native_list = CommandList->Native;
	command_queue->ExecuteCommandLists(1, &native_list);
	TWU::SuccessAssert(command_queue->Signal(fence, fence_flush_value), "TW3DCommandQueue::ExecuteCommandList"s);
}

void TW3DCommandQueue::ExecuteCommandLists(const std::vector<TW3DCommandList*>& CommandLists) {
	fence_flush_value++;

	std::vector<ID3D12CommandList*> nativeLists(CommandLists.size());
	for (TWT::uint i = 0; i < nativeLists.size(); i++) {
		nativeLists[i] = CommandLists[i]->Native;
		CommandLists[i]->SignalValue = fence_flush_value;
	}

	command_queue->ExecuteCommandLists(static_cast<UINT>(nativeLists.size()), nativeLists.data());
	TWU::SuccessAssert(command_queue->Signal(fence, fence_flush_value), "TW3DCommandQueue::ExecuteCommandLists, command_queue->Signal "s + fence_flush_value);
}

void TW3DCommandQueue::DebugBeginEvent(TWT::String const& Name, TWT::float3 Color) {
	PIXBeginEvent(command_queue, PIX_COLOR(Color.r * 255, Color.g * 255, Color.b * 255), Name.ToCharArray());
}

void TW3DCommandQueue::DebugEndEvent() {
	PIXEndEvent();
}

TW3DCommandQueue* TW3DCommandQueue::CreateDirect(TW3DDevice* Device) {
	return new TW3DCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}

TW3DCommandQueue* TW3DCommandQueue::CreateCompute(TW3DDevice* Device) {
	return new TW3DCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}

TW3DCommandQueue* TW3DCommandQueue::CreateCopy(TW3DDevice* Device) {
	return new TW3DCommandQueue(Device, D3D12_COMMAND_LIST_TYPE_COPY);
}
