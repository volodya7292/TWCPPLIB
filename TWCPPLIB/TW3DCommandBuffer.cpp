#include "pch.h"
#include "TW3DCommandBuffer.h"
#include "TW3DResourceManager.h"

TW3DCommandBuffer::TW3DCommandBuffer(TWT::String const& Name, TW3DResourceManager* ResourceManager,
	TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize) :
	resource_manager(ResourceManager), cmd_signature(CommandSignature) {

	cmd_buffer = ResourceManager->CreateBuffer(Name + "-cmd_buffer"s, MaxCommandCount, SingleCommandByteSize, false, true, D3D12_RESOURCE_STATE_COPY_DEST);
	count_buffer = ResourceManager->CreateBuffer(Name + "-count_buffer"s, 1, sizeof(TWT::uint), false, true, D3D12_RESOURCE_STATE_COPY_DEST);
}

TW3DCommandBuffer::~TW3DCommandBuffer() {
	delete cmd_buffer;
	delete count_buffer;
}

const TW3DCommandSignature* TW3DCommandBuffer::GetSignature() const {
	return cmd_signature;
}

const TW3DBuffer* TW3DCommandBuffer::GetCommandBuffer() const {
	return cmd_buffer;
}

const TW3DBuffer* TW3DCommandBuffer::GetCountBuffer() const {
	return count_buffer;
}

const TWT::uint TW3DCommandBuffer::GetMaxCommandCount() const {
	return cmd_buffer->GetMaxElementCount();
}

void TW3DCommandBuffer::Update(const void* Commands, TWT::uint CommandCount) {
	cmd_buffer->Update(Commands, CommandCount);
	count_buffer->Update(&CommandCount, 1);
}
