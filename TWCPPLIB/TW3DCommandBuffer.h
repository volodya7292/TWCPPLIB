#pragma once
#include "TW3DCommandSignature.h"
#include "TW3DBuffer.h"

class TW3DResourceManager;

class TW3DCommandBuffer {
public:
	TW3DCommandBuffer(TWT::String const& Name, TW3DResourceManager* ResourceManager,
		TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize);
	~TW3DCommandBuffer();

	const TW3DCommandSignature* GetSignature() const;
	const TW3DBuffer* GetBuffer() const;
	const TWT::uint GetCommandCount() const;

	void Update(const void* Commands, TWT::uint CommandCount);

private:
	TW3DResourceManager* resource_manager;

	TW3DBuffer* cmd_buffer;
	TW3DCommandSignature* cmd_signature;
};