#include "pch.h"
#include "TW3DCommandBuffer.h"
#include "TW3DResourceManager.h"

TW3DCommandBuffer::TW3DCommandBuffer(TWT::String const& Name, TW3DResourceManager* ResourceManager,
	TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize) :
	resource_manager(ResourceManager), cmd_signature(CommandSignature) {

	cmd_buffer = ResourceManager->CreateBuffer(Name + "-cmd_buffer"s, MaxCommandCount, SingleCommandByteSize, false, true);
}

TW3DCommandBuffer::~TW3DCommandBuffer() {
	delete cmd_buffer;
}

const TW3DCommandSignature* TW3DCommandBuffer::GetSignature() const {
	return cmd_signature;
}

const TW3DBuffer* TW3DCommandBuffer::GetBuffer() const {
	return cmd_buffer;
}

const TWT::uint TW3DCommandBuffer::GetCommandCount() const {
	return cmd_buffer->GetElementCount();
}

void TW3DCommandBuffer::Update(const void* Commands, TWT::uint CommandCount) {
	cmd_buffer->Update(Commands, CommandCount);
}
