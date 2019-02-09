#pragma once
#include "TWTypes.h"

namespace TW3D {
	class TW3DRootParameter {
	public:
		TW3DRootParameter(D3D12_ROOT_PARAMETER_TYPE Type, D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::UInt Register);
		TW3DRootParameter(D3D12_SHADER_VISIBILITY ShaderVisibility, TWT::Vector<D3D12_DESCRIPTOR_RANGE>& DescriptorRanges);
		~TW3DRootParameter() = default;

		D3D12_ROOT_PARAMETER Get();

		TWT::UInt GetIndex();
		void SetIndex(TWT::UInt Index);

		static TW3DRootParameter CreateCBV(TWT::UInt Register, D3D12_SHADER_VISIBILITY ShaderVisibility);
		//static TW3DRootParameter CreateSRV(D3D12_SHADER_VISIBILITY ShaderVisibility, const TWT::Vector<D3D12_DESCRIPTOR_RANGE>& DescriptorRanges);

	private:
		D3D12_ROOT_PARAMETER RootParameter;
		TWT::UInt Index;
	};
}