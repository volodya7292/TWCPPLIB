#include "pch.h"
#include "TW3DGraphicsPipelineState.h"

TW3DGraphicsPipelineState::TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC Sampledesc, D3D12_RASTERIZER_DESC RasterizerState,
	D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState, TW3DRootSignature* RootSignature, TWT::uint RTCount) :
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

TW3DGraphicsPipelineState::TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, TW3DRootSignature* RootSignature) :
	TW3DGraphicsPipelineState(PrimitiveTopologyType, {1, 0}, CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT), CD3DX12_BLEND_DESC(D3D12_DEFAULT), RootSignature, 0)
{
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.StencilEnable = false;
}

TW3DGraphicsPipelineState::~TW3DGraphicsPipelineState() {
	TWU::DXSafeRelease(pipeline_state);
	if (RootSignature->DestroyOnPipelineDestroy)
		delete RootSignature;
}

ID3D12PipelineState* TW3DGraphicsPipelineState::Get() {
	return pipeline_state;
}

void TW3DGraphicsPipelineState::SetVertexShader(TW3DShader* Shader) {
	desc.VS = Shader->GetByteCode();
}

void TW3DGraphicsPipelineState::SetPixelShader(TW3DShader* Shader) {
	desc.PS = Shader->GetByteCode();
}

void TW3DGraphicsPipelineState::SetRTVFormat(TWT::uint Index, DXGI_FORMAT Format) {
	desc.RTVFormats[Index] = Format;
}

void TW3DGraphicsPipelineState::SetDSVFormat(DXGI_FORMAT Format) {
	desc.DSVFormat = Format;
}

void TW3DGraphicsPipelineState::SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout) {
	desc.InputLayout.NumElements = static_cast<UINT>(InputLayout.size());
	desc.InputLayout.pInputElementDescs = InputLayout.data();
}

void TW3DGraphicsPipelineState::Create(TW3DDevice* Device) {
	Device->CreateGraphicsPipelineState(&desc, &pipeline_state);
	pipeline_state->SetName(L"TW3DGraphicsPipelineState");
}

std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout(const std::vector<TW3DInputLayoutElement>& Elements) {
	std::vector<D3D12_INPUT_ELEMENT_DESC> descs;

	for (const TW3DInputLayoutElement& element : Elements) {
		switch (element) {
		case TW3D_ILE_POSITION:
			descs.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D_ILE_TEXCOORD:
			descs.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D_ILE_TEXCOORD_3D:
			descs.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D_ILE_NORMAL:
			descs.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D_ILE_TANGENT:
			descs.push_back({ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		case TW3D_ILE_BITANGENT:
			descs.push_back({ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
			break;
		}
	}

	return descs;
}