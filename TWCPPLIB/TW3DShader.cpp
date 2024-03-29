#include "pch.h"
#include "TW3DShader.h"
#include "TW3DUtils.h"
//#include "dxc/Support/Global.h"
//#include "dxc/Support/dxcapi.use.h"
//#include "dxc/Support/HLSLOptions.h"
//#include "dxc/DxilContainer/DxilContainer.h"
//#include "dxc/DxilContainer/DxilRuntimeReflection.h"
//#include "dxc/DXIL/DxilShaderFlags.h"
//#include "dxc/DXIL/DxilUtil.h"


TW3DShader::TW3DShader(TWT::String const& Filename) :
	name(Filename) {

	TWT::uint size;
	TWT::byte* data = TWU::ReadFileBytes(Filename, size);
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	//dll_support.CreateInstance(CLSID_DxcLibrary, &idxc_library);
	//idxc_library->CreateBlobWithEncodingFromPinned((LPBYTE)data, size, CP_UTF8, &idxc_blob_encoding);

	/*idxc
	CComPtr<IDxcCompiler> pCompiler;
	CComPtr<IDxcBlobEncoding> pSource;
	CComPtr<IDxcBlob> pProgram;
	CComPtr<IDxcBlobEncoding> pDisassembly;
	CComPtr<IDxcOperationResult> pResult;
	hlsl::DxilContainerHeader *pHeader;
	hlsl::DxilPartIterator pPartIter(nullptr, 0);

	VERIFY_SUCCEEDED(CreateCompiler(&pCompiler));
	CreateBlobFromText("float4 main() : SV_Target { return 0; }", &pSource);
	VERIFY_SUCCEEDED(pCompiler->Compile(pSource, L"hlsl.hlsl", L"main", L"ps_6_0",
		nullptr, 0, nullptr, 0, nullptr,
		&pResult));
	VERIFY_SUCCEEDED(pResult->GetResult(&pProgram));*/



	init_reflection();
}

TW3DShader::TW3DShader(D3D12_SHADER_BYTECODE const& ByteCode, TWT::String const& ShaderName) :
	name(ShaderName) {

	release_bytecode_data = false;
	bytecode = ByteCode;

	init_reflection();
}

TW3DShader::~TW3DShader() {
	TWU::DXSafeRelease(reflection);

	if (release_bytecode_data)
		delete[] (TWT::byte*)bytecode.pShaderBytecode;
}

D3D12_SHADER_BYTECODE TW3DShader::GetByteCode() {
	return bytecode;
}

TWT::uint TW3DShader::GetRegister(TWT::String const& InputVariableName) {
	D3D12_SHADER_INPUT_BIND_DESC desc;
	
	TWU::SuccessAssert(reflection->GetResourceBindingDescByName(InputVariableName.ToCharArray(), &desc),
		"TW3DShader::GetRegister, GetResourceBindingDescByName \'"s + InputVariableName + "\'"s);

	return desc.BindPoint;
}

void TW3DShader::init_reflection() {
	TWU::SuccessAssert(D3DReflect(bytecode.pShaderBytecode, bytecode.BytecodeLength, IID_PPV_ARGS(&reflection)), "Reflect shader failed: "s + name);
	TWU::SuccessAssert(reflection->GetDesc(&desc), "Failed to get shader reflection desc: "s + name);	
}
