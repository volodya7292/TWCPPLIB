#pragma once
#include "TW3DRootSignature.h"
#include "TW3DTypes.h"

namespace TW3D {
	enum InputLayoutElement {
		POSITION_ILE,
		TEXCOORD_ILE,
		NORMAL_ILE
	};

	class TW3DGraphicsPipelineState {
	public:
		TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC SampleDesc, D3D12_RASTERIZER_DESC RasterizerState,
			D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState, TW3DRootSignature* RootSignature, TWT::UInt RTCount);
		TW3DGraphicsPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, TW3DRootSignature* RootSignature);
		~TW3DGraphicsPipelineState();

		ID3D12PipelineState* Get();

		void SetVertexShader(const std::string& Filename);
		void SetVertexShader(D3D12_SHADER_BYTECODE ByteCode);
		void SetPixelShader(const std::string& Filename);
		void SetPixelShader(D3D12_SHADER_BYTECODE ByteCode);
		void SetRTVFormat(TWT::UInt Index, DXGI_FORMAT Format);
		void SetDSVFormat(DXGI_FORMAT Format);
		void SetInputLayout(const TWT::Vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout);

		void Create(TW3DDevice* Device);

		TW3DRootSignature* RootSignature;

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		ID3D12PipelineState* pipeline_state;
	};


	TWT::Vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout(TWT::Vector<InputLayoutElement> Elements);
}