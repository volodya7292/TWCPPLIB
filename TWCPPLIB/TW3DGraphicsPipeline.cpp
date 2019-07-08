#include "pch.h"
#include "TW3DGraphicsPipeline.h"

TW3DGraphicsPipeline::TW3DGraphicsPipeline(TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC Sampledesc,
	D3D12_RASTERIZER_DESC RasterizerState, D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState) :
	RootSignature(RootSignature) {

	desc.PrimitiveTopologyType = PrimitiveTopologyType;
	desc.SampleDesc = Sampledesc;
	desc.RasterizerState = RasterizerState;
	desc.DepthStencilState = DepthStencilState;
	desc.BlendState = BlendState;
	desc.pRootSignature = RootSignature->Native;
	desc.NumRenderTargets = 1;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleMask = 0xffffffff;
}

TW3DGraphicsPipeline::TW3DGraphicsPipeline(TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType) :
	TW3DGraphicsPipeline(RootSignature, PrimitiveTopologyType, { 1, 0 }, CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
		CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT), CD3DX12_BLEND_DESC(D3D12_DEFAULT)) {
	desc.DepthStencilState.DepthEnable = false;
	desc.DepthStencilState.StencilEnable = false;
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
}

TW3DGraphicsPipeline::~TW3DGraphicsPipeline() {
	TWU::DXSafeRelease(Native);
}

void TW3DGraphicsPipeline::SetVertexShader(TW3DShader* Shader) {
	desc.VS = Shader->GetByteCode();
}

void TW3DGraphicsPipeline::SetGeometryShader(TW3DShader* Shader) {
	desc.GS = Shader->GetByteCode();
}

void TW3DGraphicsPipeline::SetPixelShader(TW3DShader* Shader) {
	desc.PS = Shader->GetByteCode();
}

void TW3DGraphicsPipeline::SetRTVFormat(TWT::uint Index, DXGI_FORMAT Format) {
	desc.RTVFormats[Index] = Format;
	desc.NumRenderTargets = Index + 1;
}

void TW3DGraphicsPipeline::SetDSVFormat(DXGI_FORMAT Format) {
	desc.DSVFormat = Format;
}

void TW3DGraphicsPipeline::SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout) {
	desc.InputLayout.NumElements = static_cast<UINT>(InputLayout.size());
	desc.InputLayout.pInputElementDescs = InputLayout.data();
}

void TW3DGraphicsPipeline::Create(TW3DDevice* Device) {
	Device->CreateGraphicsPipelineState(&desc, &Native);
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