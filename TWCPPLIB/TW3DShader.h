#pragma once
#include "TWTypes.h"

class TW3DShader {
public:
	TW3DShader(TWT::String const& Filename);
	TW3DShader(D3D12_SHADER_BYTECODE const& ByteCode, TWT::String const& ShaderName);
	~TW3DShader();

	D3D12_SHADER_BYTECODE GetByteCode();
	TWT::uint GetRegister(TWT::String const& InputVariableName);

private:
	void init_reflection();

	TWT::String name;
	ID3D12ShaderReflection* reflection;
	D3D12_SHADER_BYTECODE bytecode;
	D3D12_SHADER_DESC desc;
	bool release_bytecode_data = true;
};