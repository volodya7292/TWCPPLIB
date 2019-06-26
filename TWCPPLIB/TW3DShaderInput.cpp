#include "pch.h"
#include "TW3DShaderInput.h"
#include "TW3DTexture.h"
#include "TW3DConstantBuffer.h"

TW3DShaderInput::TW3DShaderInput() {
}

TW3DShaderBinding& TW3DShaderInput::operator[](TWT::String const& BindingName) {
	return bindings[BindingName];
}


TW3DShaderBinding::TW3DShaderBinding() {
	bind_desc.BindPoint = -1;
}

TW3DShaderBinding::TW3DShaderBinding(TW3DCommandList* CommandList,
	D3D12_SHADER_INPUT_BIND_DESC BindDesc, ID3D11ShaderReflectionConstantBuffer* CBReflection, TWT::uint VariableBit32Offset) :
	command_list(CommandList), bind_desc(BindDesc), cb(CBReflection), variable_bit32_offset(VariableBit32Offset)
{
}

TW3DShaderBinding& TW3DShaderBinding::operator[](TWT::String const& VariableName) {
	return TW3DShaderBinding(command_list, bind_desc, nullptr, cb->GetVariableByName(VariableName.ToCharArray())->GetInterfaceSlot(0));
}

TW3DShaderBinding& TW3DShaderBinding::operator=(TW3DResource* Resource) {
	switch (bind_desc.Type) {
	case D3D_SIT_CBUFFER:
		command_list->BindConstantBuffer(bind_desc.BindPoint, dynamic_cast<TW3DConstantBuffer*>(Resource), 0);
		break;
	case D3D_SIT_TEXTURE:
		command_list->BindTexture(bind_desc.BindPoint, dynamic_cast<TW3DTexture*>(Resource));
		break;
	case D3D_SIT_UAV_RWTYPED:
		command_list->BindTexture(bind_desc.BindPoint, dynamic_cast<TW3DTexture*>(Resource), true);
		break;
	case D3D_SIT_STRUCTURED:
		command_list->BindBuffer(bind_desc.BindPoint, Resource);
		break;
	case D3D_SIT_UAV_RWSTRUCTURED:
		command_list->BindBuffer(bind_desc.BindPoint, Resource, true);
		break;
	default:
		TWU::TW3DLogError("Binding type not implemented : "s + static_cast<TWT::uint>(bind_desc.Type));
		break;
	}

	return *this;
}

TW3DShaderBinding& TW3DShaderBinding::operator=(TWT::uint Data) {
	command_list->BindUIntConstant(bind_desc.BindPoint, Data, variable_bit32_offset);
	return *this;
}
