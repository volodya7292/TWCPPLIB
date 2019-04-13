#include "pch.h"
#include "TW3DGraphicsPipelineState.h"

TW3D::TW3DGraphicsPipelineState::TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC Sampledesc, D3D12_RASTERIZER_DESC RasterizerState,
	D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState, TW3DRootSignature* RootSignature, TWT::UInt RTCount) :
	RootSignature(RootSignature)
{
	desc.PrimitiveTopologyType = PrimitiveTopologyType;
	desc.SampleDesc = Sampledesc;
	desc.RasterizerState = RasterizerState;
	desc.DepthStencilState = DepthStencilState;
	desc.BlendState = BlendState;
	desc.pRootSignature = RootSignature->Get();
	desc.NumRenderTargets = RTCount;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleMask = 0xffffffff;
}

TW3D::TW3DGraphicsPipelineState::~TW3DGraphicsPipelineState() {
	TWU::DXSafeRelease(pipeline_state);
	delete RootSignature;
}

ID3D12PipelineState* TW3D::TW3DGraphicsPipelineState::Get() {
	return pipeline_state;
}

void TW3D::TW3DGraphicsPipelineState::SetVertexShader(const std::string& Filename) {
	TWT::Int size;
	TWT::Byte* data = TWU::ReadFileBytes(Filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	desc.VS = bytecode;
}

void TW3D::TW3DGraphicsPipelineState::SetPixelShader(const std::string& Filename) {
	TWT::Int size;
	TWT::Byte* data = TWU::ReadFileBytes(Filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	desc.PS = bytecode;
}

void TW3D::TW3DGraphicsPipelineState::SetRTVFormat(TWT::UInt Index, DXGI_FORMAT Format) {
	desc.RTVFormats[Index] = Format;
}

void TW3D::TW3DGraphicsPipelineState::SetDSVFormat(DXGI_FORMAT Format) {
	desc.DSVFormat = Format;
}

void TW3D::TW3DGraphicsPipelineState::SetInputLayout(const TWT::Vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout) {
	D3D12_INPUT_LAYOUT_DESC inputLayoutdesc = {};
	inputLayoutdesc.NumElements = static_cast<UINT>(InputLayout.size());
	inputLayoutdesc.pInputElementDescs = InputLayout.data();

	desc.InputLayout = inputLayoutdesc;
}

void TW3D::TW3DGraphicsPipelineState::Create(TW3DDevice* Device) {
	Device->CreateGraphicsPipelineState(&desc, &pipeline_state);
}
