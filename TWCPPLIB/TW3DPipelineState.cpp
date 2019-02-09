#include "pch.h"
#include "TW3DPipelineState.h"

TW3D::TW3DPipelineState::TW3DPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC SampleDesc, D3D12_RASTERIZER_DESC RasterizerState,
	D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState, TW3DRootSignature* RootSignature, TWT::UInt Count) {

	Desc.PrimitiveTopologyType = PrimitiveTopologyType;
	Desc.SampleDesc = SampleDesc;
	Desc.RasterizerState = RasterizerState;
	Desc.DepthStencilState = DepthStencilState;
	Desc.BlendState = BlendState;
	Desc.pRootSignature = RootSignature->Get();
	Desc.NumRenderTargets = Count;
	Desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	Desc.SampleMask = 0xffffffff;
}

TW3D::TW3DPipelineState::~TW3DPipelineState() {
	TWU::DXSafeRelease(PipelineState);
}

ID3D12PipelineState* TW3D::TW3DPipelineState::Get() {
	return PipelineState;
}

void TW3D::TW3DPipelineState::SetVertexShader(const std::string& filename) {
	TWT::Int size;
	TWT::Byte* data = TWU::ReadFileBytes(filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	Desc.VS = bytecode;
}

void TW3D::TW3DPipelineState::SetPixelShader(const std::string& filename) {
	TWT::Int size;
	TWT::Byte* data = TWU::ReadFileBytes(filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	Desc.PS = bytecode;
}

void TW3D::TW3DPipelineState::SetRTVFormat(TWT::UInt Index, DXGI_FORMAT Format) {
	Desc.RTVFormats[Index] = Format;
}

void TW3D::TW3DPipelineState::SetDSVFormat(DXGI_FORMAT Format) {
	Desc.DSVFormat = Format;
}

void TW3D::TW3DPipelineState::SetInputLayout(const TWT::Vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout) {
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = InputLayout.size();
	inputLayoutDesc.pInputElementDescs = InputLayout.data();

	Desc.InputLayout = inputLayoutDesc;
}

void TW3D::TW3DPipelineState::Create(TW3DDevice* Device) {
	Device->CreateGraphicsPipelineState(&Desc, &PipelineState);
}
