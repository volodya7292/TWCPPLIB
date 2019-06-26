#pragma once
#include "TW3DResource.h"

class TW3DShaderBinding {
public:
	TW3DShaderBinding();
	TW3DShaderBinding(TW3DCommandList* CommandList, D3D12_SHADER_INPUT_BIND_DESC BindDesc,
		ID3D11ShaderReflectionConstantBuffer* CBReflection = nullptr, TWT::uint VariableBit32Offset = 0);

	TW3DShaderBinding& operator [] (TWT::String const& VariableName);
	TW3DShaderBinding& operator =  (TW3DResource* Resource);
	TW3DShaderBinding& operator =  (TWT::uint Data);

private:
	TW3DCommandList* command_list = nullptr;
	
	D3D12_SHADER_INPUT_BIND_DESC bind_desc;
	ID3D11ShaderReflectionConstantBuffer* cb = nullptr;
	TWT::uint variable_bit32_offset = -1;
};

class TW3DShaderInput {
public:
	TW3DShaderInput();

	TW3DShaderBinding& operator [] (TWT::String const& BindingName);

private:
	std::unordered_map<TWT::String, TW3DShaderBinding> bindings;
	//std::unordered_map;
	//std:D3D12_SHADER_INPUT_BIND_DESC
};

