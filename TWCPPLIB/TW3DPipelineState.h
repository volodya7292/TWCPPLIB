#pragma once
#include "TW3DRootSignature.h"

namespace TW3D {
	class TW3DPipelineState {
	public:
		TW3DPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType, DXGI_SAMPLE_DESC SampleDesc, D3D12_RASTERIZER_DESC RasterizerState,
			D3D12_DEPTH_STENCIL_DESC DepthStencilState, D3D12_BLEND_DESC BlendState, TW3DRootSignature* RootSignature, TWT::UInt Count);
		~TW3DPipelineState();

		ID3D12PipelineState* Get();

		void SetVertexShader(const std::string& filename);
		void SetPixelShader(const std::string& filename);
		void SetRTVFormat(TWT::UInt Index, DXGI_FORMAT Format);
		void SetDSVFormat(DXGI_FORMAT Format);
		void SetInputLayout(const TWT::Vector<D3D12_INPUT_ELEMENT_DESC>& InputLayout);

		void Create(TW3DDevice* Device);

	private:
		D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc = {};
		ID3D12PipelineState* PipelineState;
	};
}