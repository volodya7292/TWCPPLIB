#include "pch.h"
#include "TW3DCommandSignature.h"

TW3DCommandSignature::TW3DCommandSignature(TW3DDevice* Device, std::vector<TW3DCSCommandArgument> const& CommandArguments, TW3DRootSignature* RootSignature) {
	D3D12_COMMAND_SIGNATURE_DESC desc = {};
	std::vector<D3D12_INDIRECT_ARGUMENT_DESC> native_args(CommandArguments.size());

	for (auto& arg : CommandArguments) {
		native_args[arg.Index] = arg.Parameter;
		desc.ByteStride += arg.ByteSize;
	}
	desc.ByteStride = TWT::Max(desc.ByteStride, 32u);

	desc.NumArgumentDescs = CommandArguments.size();
	desc.pArgumentDescs = native_args.data();

	Device->CreateCommandSignature(&desc, RootSignature ? RootSignature->Native : nullptr, &Native);
}

TW3DCommandSignature::~TW3DCommandSignature() {
	TWU::DXSafeRelease(Native);
}

TW3DCSCommandArgument TW3DCADraw(TWT::uint Index) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

	return { Index, sizeof(D3D12_DRAW_ARGUMENTS), arg_desc };
}

TW3DCSCommandArgument TW3DCADrawIndexed(TWT::uint Index) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	return { Index, sizeof(D3D12_DRAW_INDEXED_ARGUMENTS), arg_desc };
}

TW3DCSCommandArgument TW3DCADispatch(TWT::uint Index) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;

	return { Index, sizeof(D3D12_DISPATCH_ARGUMENTS), arg_desc };
}

TW3DCSCommandArgument TW3DCAVertexBuffer(TWT::uint Index, TWT::uint Slot) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_VERTEX_BUFFER_VIEW;
	arg_desc.VertexBuffer.Slot = Slot;

	return { Index, sizeof(D3D12_VERTEX_BUFFER_VIEW), arg_desc };
}

TW3DCSCommandArgument TW3DCAIndexBuffer(TWT::uint Index) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_INDEX_BUFFER_VIEW;

	return { Index, sizeof(D3D12_INDEX_BUFFER_VIEW), arg_desc };
}

TW3DCSCommandArgument TW3DCAConstant(TWT::uint Index, TWT::uint RootParameterIndex, TWT::uint DestOffsetIn32BitValues) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
	arg_desc.Constant.RootParameterIndex = RootParameterIndex;
	arg_desc.Constant.DestOffsetIn32BitValues = DestOffsetIn32BitValues;
	arg_desc.Constant.Num32BitValuesToSet = 1;

	return { Index, sizeof(TWT::uint), arg_desc };
}

TW3DCSCommandArgument TW3DCAConstants(TWT::uint Index, TWT::uint RootParameterIndex, TWT::uint DestOffsetIn32BitValues, TWT::uint Num32BitValuesToSet) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
	arg_desc.Constant.RootParameterIndex = RootParameterIndex;
	arg_desc.Constant.DestOffsetIn32BitValues = DestOffsetIn32BitValues;
	arg_desc.Constant.Num32BitValuesToSet = Num32BitValuesToSet;

	return { Index, Num32BitValuesToSet * sizeof(TWT::uint), arg_desc };
}

TW3DCSCommandArgument TW3DCAConstantBuffer(TWT::uint Index, TWT::uint RootParameterIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT_BUFFER_VIEW;
	arg_desc.ConstantBufferView.RootParameterIndex = RootParameterIndex;

	return { Index, sizeof(D3D12_GPU_VIRTUAL_ADDRESS), arg_desc };
}

TW3DCSCommandArgument TW3DCAShaderResource(TWT::uint Index, TWT::uint RootParameterIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
	arg_desc.ShaderResourceView.RootParameterIndex = RootParameterIndex;

	return { Index, sizeof(D3D12_GPU_VIRTUAL_ADDRESS), arg_desc };
}

TW3DCSCommandArgument TW3DCAUnorderedAccessResource(TWT::uint Index, TWT::uint RootParameterIndex) {
	D3D12_INDIRECT_ARGUMENT_DESC arg_desc = {};
	arg_desc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
	arg_desc.UnorderedAccessView.RootParameterIndex = RootParameterIndex;

	return { Index, sizeof(D3D12_GPU_VIRTUAL_ADDRESS), arg_desc };
}