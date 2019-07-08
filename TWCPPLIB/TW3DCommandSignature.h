#pragma once
#include "TW3DRootSignature.h"

struct TW3DCSCommandArgument {
	TWT::uint Index;
	TWT::uint ByteSize;
	D3D12_INDIRECT_ARGUMENT_DESC Parameter;
};

class TW3DCommandSignature {
public:
	TW3DCommandSignature(TW3DDevice* Device, TW3DRootSignature* RootSignature, std::vector<TW3DCSCommandArgument> const& CommandArguments);
	~TW3DCommandSignature();

	ID3D12CommandSignature* Native;
};

TW3DCSCommandArgument TW3DCADraw(TWT::uint Index);
TW3DCSCommandArgument TW3DCADrawIndexed(TWT::uint Index);
TW3DCSCommandArgument TW3DCADispatch(TWT::uint Index);
TW3DCSCommandArgument TW3DCAVertexBuffer(TWT::uint Index, TWT::uint Slot = 0);
TW3DCSCommandArgument TW3DCAIndexBuffer(TWT::uint Index);
TW3DCSCommandArgument TW3DCAConstant(TWT::uint Index, TWT::uint RootParameterIndex, TWT::uint DestOffsetIn32BitValues);
TW3DCSCommandArgument TW3DCAConstants(TWT::uint Index, TWT::uint RootParameterIndex, TWT::uint DestOffsetIn32BitValues, TWT::uint Num32BitValuesToSet);
TW3DCSCommandArgument TW3DCAConstantBuffer(TWT::uint Index, TWT::uint RootParameterIndex);
TW3DCSCommandArgument TW3DCAShaderResource(TWT::uint Index, TWT::uint RootParameterIndex);
TW3DCSCommandArgument TW3DCAUnorderedAccessResource(TWT::uint Index, TWT::uint RootParameterIndex);