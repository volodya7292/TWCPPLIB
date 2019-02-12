#pragma once
#include "TWTypes.h"

namespace TW3D {
	class TW3DRootParameter {
	public:
		TW3DRootParameter(D3D12_ROOT_PARAMETER_TYPE Type, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register);
		TW3DRootParameter(D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::Vector<D3D12_DESCRIPTOR_RANGE>& DescriptorRanges, TWT::UInt IndexOffset);
		TW3DRootParameter() = default;
		~TW3DRootParameter() = default;

		D3D12_ROOT_PARAMETER Get();

		static TW3DRootParameter CreateCBV(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility);

	private:
		D3D12_ROOT_PARAMETER RootParameter;
	};
}