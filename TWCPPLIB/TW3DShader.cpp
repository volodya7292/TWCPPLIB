#include "pch.h"
#include "TW3DShader.h"
#include "TW3DUtils.h"

TW3DShader::TW3DShader(TWT::String const& Filename) {
	TWT::uint size;
	TWT::byte* data = TWU::ReadFileBytes(Filename, size);
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;
	
	TWU::SuccessAssert(D3DReflect(data, size, IID_PPV_ARGS(&reflection)), "Reflect shader failed: "s + Filename);
}

TW3DShader::TW3DShader(D3D12_SHADER_BYTECODE const& ByteCode, TWT::String const& ShaderName) {
	release_bytecode_data = false;
	bytecode = ByteCode;
	TWU::SuccessAssert(D3DReflect(ByteCode.pShaderBytecode, ByteCode.BytecodeLength, IID_PPV_ARGS(&reflection)), "Reflect shader failed: "s + ShaderName);
}

TW3DShader::~TW3DShader() {
	TWU::DXSafeRelease(reflection);

	if (release_bytecode_data)
		delete[] bytecode.pShaderBytecode;
}

D3D12_SHADER_BYTECODE TW3DShader::GetByteCode() {
	return bytecode;
}

TWT::uint TW3DShader::GetRegister(TWT::String InputVariableName) {
	D3D12_SHADER_INPUT_BIND_DESC desc;
	TWU::SuccessAssert(reflection->GetResourceBindingDescByName(InputVariableName.ToCharArray(), &desc),
		"TW3DShader::GetRegister, GetResourceBindingDescByName \'"s + InputVariableName + "\'"s);

	return desc.BindPoint;
}