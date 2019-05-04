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

TW3D::TW3DGraphicsPipelineState::TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, TW3DRootSignature* RootSignature) :
	TW3DGraphicsPipelineState(PrimitiveTopologyType, {1, 0}, CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT), CD3DX12_BLEND_DESC(D3D12_DEFAULT), RootSignature, 0)
{
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.StencilEnable = false;
}

TW3D::TW3DGraphicsPipelineState::~TW3DGraphicsPipelineState() {
	TWU::DXSafeRelease(pipeline_state);
	if (RootSignature->DestroyOnPipelineDestroy)
		delete RootSignature;
}

ID3D12PipelineState* TW3D::TW3DGraphicsPipelineState::Get() {
	return pipeline_state;
}

void TW3D::TW3DGraphicsPipelineState::SetVertexShader(const std::string& Filename) {
	TWT::UInt size;
	TWT::Byte* data = TWU::ReadFileBytes(Filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	desc.VS = bytecode;
}

void TW3D::TW3DGraphicsPipelineState::SetVertexShader(const D3D12_SHADER_BYTECODE& ByteCode) {
	desc.VS = ByteCode;
}

void TW3D::TW3DGraphicsPipelineState::SetPixelShader(const std::string& Filename) {
	TWT::UInt size;
	TWT::Byte* data = TWU::ReadFileBytes(Filename, size);

	D3D12_SHADER_BYTECODE bytecode = {};
	bytecode.BytecodeLength = size;
	bytecode.pShaderBytecode = data;

	desc.PS = bytecode;
}

void TW3D::TW3DGraphicsPipelineState::SetPixelShader(const D3D12_SHADER_BYTECODE& ByteCode) {
	desc.PS = ByteCode;
}

void TW3D::TW3DGraphicsPipelineState::SetRTVFormat(TWT::UInt Index, DXGI_FORMAT Format) {
	desc.RTVFormats[Index] = Format;
}

void TW3D::TW3DGraphicsPipelineState::SetDSVFormat(DXGI_FORMAT Format) {
	desc.DSVFormat = Format;
}

void TW3D::TW3DGraphicsPipelineState::SetInputLayout(const TWT::Vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout) {
	desc.InputLayout.NumElements = static_cast<UINT>(InputLayout.size());
	desc.InputLayout.pInputElementDescs = InputLayout.data();
}

void TW3D::TW3DGraphicsPipelineState::Create(TW3DDevice* Device) {
	Device->CreateGraphicsPipelineState(&desc, &pipeline_state);
	pipeline_state->SetName(L"TW3DGraphicsPipelineState");
}

TWT::Vector<D3D12_INPUT_ELEMENT_DESC> TW3D::CreateInputLayout(const TWT::Vector<InputLayoutElement>& Elements) {
	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> descs;
	for (const InputLayoutElement& element : Elements) {
		switch (element) {
		case TW3D::POSITION_ILE:
			descs.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D::TEXCOORD_ILE:
			descs.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D::NORMAL_ILE:
			descs.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		}
	}

	return descs;
}