#include "pch.h"
#include "TW3DFence.h"

TW3D::TW3DFence::TW3DFence(TW3D::TW3DDevice* device) {
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, &fence);
}

TW3D::TW3DFence::~TW3DFence() {
	TWU::DXSafeRelease(fence);
}

void TW3D::TW3DFence::SetFlushValue(TWT::UInt64 value) {
	flushValue = value;
}

void TW3D::TW3DFence::Flush(TW3DCommandQueue* commandQueue) {
	commandQueue->SignalFence(fence, ++flushValue);

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (fence->GetCompletedValue() < flushValue) {
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		TWU::ThrowIfFailed(fence->SetEventOnCompletion(flushValue, fenceEvent));

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

ID3D12Fence1* TW3D::TW3DFence::Get() {
	return fence;
}
