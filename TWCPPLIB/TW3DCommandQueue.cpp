#include "pch.h"
#include "TW3DCommandQueue.h"

TW3D::TW3DCommandQueue::TW3DCommandQueue(TW3DDevice* device, D3D12_COMMAND_LIST_TYPE type) {
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Type = type;

	device->CreateCommandQueue(&desc, &commandQueue);
}

TW3D::TW3DCommandQueue::~TW3DCommandQueue() {
	TWU::DXSafeRelease(commandQueue);
}

ID3D12CommandQueue* TW3D::TW3DCommandQueue::Get() {
	return commandQueue;
}

void TW3D::TW3DCommandQueue::ExecuteCommandList(TW3DGraphicsCommandList* commandList) {
	ID3D12CommandList* native_list = commandList->Get();
	commandQueue->ExecuteCommandLists(1, &native_list);
}

void TW3D::TW3DCommandQueue::ExecuteCommandLists(TWT::Vector<TW3DGraphicsCommandList*> commandLists) {
	TWT::Vector<ID3D12CommandList*> nativeLists(commandLists.size());

	for (TWT::UInt i = 0; i < nativeLists.size(); i++)
		nativeLists[i] = commandLists[i]->Get();

	commandQueue->ExecuteCommandLists(static_cast<UINT>(nativeLists.size()), nativeLists.data());
}

void TW3D::TW3DCommandQueue::SignalFence(ID3D12Fence1* fence, TWT::UInt64 value) {
	TWU::SuccessAssert(commandQueue->Signal(fence, value));
}

TW3D::TW3DCommandQueue* TW3D::TW3DCommandQueue::CreateDirect(TW3DDevice* device) {
	return new TW3DCommandQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
}
