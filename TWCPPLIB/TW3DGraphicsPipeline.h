#pragma once
#include "TW3DRootSignature.h"
#include "TW3DShader.h"
#include "TW3DTypes.h"

enum TW3DInputLayoutElement {
	TW3D_ILE_POSITION,
	TW3D_ILE_TEXCOORD,
	TW3D_ILE_TEXCOORD_3D,
	TW3D_ILE_NORMAL,
	TW3D_ILE_TANGENT,
	TW3D_ILE_BITANGENT,
};

class TW3DGraphicsPipeline {
public:
	TW3DGraphicsPipeline(TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC SampleDesc,
		D3D12_RASTERIZER_DESC RasterizerState, D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState);
	TW3DGraphicsPipeline(TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	~TW3DGraphicsPipeline();

	void SetVertexShader(TW3DShader* Shader);
	void SetGeometryShader(TW3DShader* Shader);
	void SetPixelShader(TW3DShader* Shader);
	void SetRTVFormat(TWT::uint Index, DXGI_FORMAT Format);
	void SetDSVFormat(DXGI_FORMAT Format);
	void SetInputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout);

	void Create(TW3DDevice* Device);

	ID3D12PipelineState* Native = nullptr;
	TW3DRootSignature* RootSignature;

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC           desc = {};
};


std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout(const std::vector<TW3DInputLayoutElement>& Elements);
