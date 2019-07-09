#include "pch.h"
#include "TW3DResourceManager.h"

TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device) {
	temp_gcl = new TW3DTempGCL(device);
	rtv_descriptor_heap = TW3DDescriptorHeap::CreateForRTV(device, 16);
	dsv_descriptor_heap = TW3DDescriptorHeap::CreateForDSV(device, 4);
	srv_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 128);
	uav_descriptor_heap = TW3DDescriptorHeap::CreateForSR(device, 128, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	direct_command_queue = TW3DCommandQueue::CreateDirect(device);
	compute_command_queue = TW3DCommandQueue::CreateCompute(device);
	copy_command_queue = TW3DCommandQueue::CreateCopy(device);
	temp_direct_cl = RequestCommandList("_RM_TEMP_DIRECT"s, TW3D_CL_DIRECT);
	temp_compute_cl = RequestCommandList("_RM_TEMP_COMPUTE"s, TW3D_CL_COMPUTE);
	temp_copy_cl = RequestCommandList("_RM_TEMP_COPY"s, TW3D_CL_COPY);
}

TW3DResourceManager::~TW3DResourceManager() {
	for (auto [name, s] : shaders)
		delete s;
	for (auto [name, rs] : root_signatures)
		delete rs;
	for (auto [name, cs] : command_signatures)
		delete cs;
	for (auto [name, cl] : command_lists)
		delete cl;
	for (auto [name, rt] : render_targets)
		delete rt;
	for (auto [name, tex] : textures)
		delete tex;
	for (auto [name, buf] : buffers)
		delete buf;
	for (auto [name, v_buf] : vertex_buffers)
		delete v_buf;
	for (auto [name, c_buf] : constant_buffers)
		delete c_buf;
	for (auto [name, c_buf] : command_buffers)
		delete c_buf;
	for (auto [name, f_buf] : framebuffers)
		delete f_buf;
	for (auto [name, gp] : graphics_pipelines)
		delete gp;
	for (auto [name, cp] : compute_pipelines)
		delete cp;

	delete temp_gcl;
	delete rtv_descriptor_heap;
	delete dsv_descriptor_heap;
	delete srv_descriptor_heap;
	delete uav_descriptor_heap;
	delete direct_command_queue;
	delete compute_command_queue;
	delete copy_command_queue;
}

TW3DFramebuffer* TW3DResourceManager::CreateFramebuffer(TWT::uint2 Size) {
	return new TW3DFramebuffer(this, Size);
}

TW3DCommandBuffer* TW3DResourceManager::CreateCommandBuffer(TWT::String const& Name, TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize) {
	return new TW3DCommandBuffer(Name, this, CommandSignature, MaxCommandCount, SingleCommandByteSize);
}

TW3DRenderTarget* TW3DResourceManager::CreateRenderTarget(ID3D12Resource* Buffer) {
	auto* rtv = new TW3DRenderTarget(device, rtv_descriptor_heap, srv_descriptor_heap);
	rtv->Create(Buffer);
	return rtv;
}

TW3DRenderTarget* TW3DResourceManager::CreateRenderTarget(TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	auto* rtv = new TW3DRenderTarget(device, rtv_descriptor_heap, srv_descriptor_heap, Format, ClearValue);
	rtv->Create(Size);
	return rtv;
}

TW3DRenderTarget* TW3DResourceManager::CreateRenderTargetCube(TWT::uint Size, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	auto* rtv = new TW3DRenderTarget(device, rtv_descriptor_heap, srv_descriptor_heap, Format, ClearValue);
	rtv->CreateCube(Size);
	return rtv;
}

TW3DBuffer* TW3DResourceManager::CreateBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes,
	bool UAV, bool UpdateOptimized, D3D12_RESOURCE_STATES InitialState) {

	auto* buffer = new TW3DBuffer(device, temp_gcl, UpdateOptimized, ElementSizeInBytes, UAV, srv_descriptor_heap);
	buffer->InitialState = InitialState;
	buffer->Create(ElementCount);
	TWU::DXSetName(buffer->Native, Name);
	return buffer;
}

TW3DVertexBuffer* TW3DResourceManager::CreateVertexBuffer(TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes, bool OptimizeForUpdating) {
	return new TW3DVertexBuffer(device, temp_gcl, OptimizeForUpdating, VertexCount, SingleVertexSizeInBytes);
}

TW3DConstantBuffer* TW3DResourceManager::CreateConstantBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes) {
	auto buffer = new TW3DConstantBuffer(device, ElementCount, ElementSizeInBytes);
	TWU::DXSetName(buffer->Native, Name);
	return buffer;
}

TW3DTexture* TW3DResourceManager::CreateDepthStencilTexture(TWT::uint2 Size) {
	auto* texture = new TW3DTexture(device, temp_gcl, DXGI_FORMAT_D32_FLOAT, nullptr, nullptr, dsv_descriptor_heap);
	texture->CreateDepthStencil(Size);
	return texture;
}

TW3DTexture* TW3DResourceManager::CreateDepthStencilCubeTexture(TWT::uint Size, D3D12_RESOURCE_FLAGS Flags, D3D12_DSV_FLAGS DSVFlags) {
	auto* texture = new TW3DTexture(device, temp_gcl, DXGI_FORMAT_D32_FLOAT, nullptr, nullptr, dsv_descriptor_heap);
	texture->ResourceFlags = Flags;
	texture->DSVFlags = DSVFlags;
	texture->CreateDepthStencilCube(Size);
	return texture;
}

TW3DTexture* TW3DResourceManager::CreateTexture2D(const TWT::WString& Filename) {
	return TW3DTexture::Create2D(device, temp_gcl, srv_descriptor_heap, Filename);
}

TW3DTexture* TW3DResourceManager::CreateTexture2D(TWT::uint2 Size, DXGI_FORMAT Format, bool UAV) {
	auto* texture = new TW3DTexture(device, temp_gcl, Format, srv_descriptor_heap, UAV ? uav_descriptor_heap : nullptr, nullptr);
	texture->Create2D(Size);
	return texture;
}

TW3DTexture* TW3DResourceManager::CreateTextureArray2D(TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV) {
	auto* texture = new TW3DTexture(device, temp_gcl, Format, srv_descriptor_heap, UAV ? uav_descriptor_heap : nullptr, nullptr);
	texture->CreateArray2D(Size, Depth);
	return texture;
}

TW3DCommandList* TW3DResourceManager::CreateCommandList(TW3DCommandListType Type) {
	switch (Type) {
	case TW3D_CL_DIRECT:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	case TW3D_CL_COMPUTE:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
	case TW3D_CL_COPY:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COPY);
	case TW3D_CL_BUNDLE:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_BUNDLE);
	}

	return nullptr;
}

TW3DCommandList* TW3DResourceManager::CreateCommandList(TW3DCommandListType Type, TW3DGraphicsPipeline* InitialState) {
	switch (Type) {
	case TW3D_CL_DIRECT:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT, InitialState);
	case TW3D_CL_COMPUTE:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, InitialState);
	case TW3D_CL_COPY:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COPY, InitialState);
	case TW3D_CL_BUNDLE:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, InitialState);
	}

	return nullptr;
}

TW3DCommandList* TW3DResourceManager::CreateCommandList(TW3DCommandListType Type, TW3DComputePipeline* InitialState) {
	switch (Type) {
	case TW3D_CL_DIRECT:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT, InitialState);
	case TW3D_CL_COMPUTE:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, InitialState);
	case TW3D_CL_COPY:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_COPY, InitialState);
	case TW3D_CL_BUNDLE:
		return new TW3DCommandList(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, InitialState);
	}

	return nullptr;
}

TW3DCommandList* TW3DResourceManager::GetTemporaryDirectCommandList() {
	temp_direct_cl->Reset();
	temp_direct_cl->BindResources(this);
	return temp_direct_cl;
}

TW3DCommandList* TW3DResourceManager::GetTemporaryComputeCommandList() {
	temp_compute_cl->Reset();
	temp_compute_cl->BindResources(this);
	return temp_compute_cl;
}

TW3DCommandList* TW3DResourceManager::GetTemporaryCopyCommandList() {
	temp_copy_cl->Reset();
	return temp_copy_cl;
}

TW3DCommandQueue* TW3DResourceManager::GetCommandListCommandQueue(TW3DCommandList* CommandList) {
	switch (CommandList->GetType()) {
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		return compute_command_queue;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		return copy_command_queue;
	default:
		return direct_command_queue;
	}
}

TW3DShader* TW3DResourceManager::RequestShader(TWT::String const& Name, D3D12_SHADER_BYTECODE const& ByteCode) {
	auto& shader = shaders[Name];

	if (!shader) {
		shader = new TW3DShader(ByteCode, Name);
	}

	return shader;
}

TW3DRootSignature* TW3DResourceManager::RequestRootSignature(TWT::String const& Name, std::vector<TW3DRSRootParameter> const& RootParameters, bool VS, bool PS, bool GS, bool IA) {
	auto& rs = root_signatures[Name];

	if (!rs) {
		rs = new TW3DRootSignature(device, RootParameters, VS, PS, GS, IA);
		TWU::DXSetName(rs->Native, Name);
	}

	return rs;
}

TW3DRootSignature* TW3DResourceManager::RequestRootSignature(TWT::String const& Name, std::vector<TW3DRSRootParameter> const& RootParameters,
	std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers, bool VS, bool PS, bool GS, bool IA) {

	auto& rs = root_signatures[Name];

	if (!rs) {
		rs = new TW3DRootSignature(device, RootParameters, StaticSamplers, VS, PS, GS, IA);
		TWU::DXSetName(rs->Native, Name);
	}

	return rs;
}

TW3DCommandSignature* TW3DResourceManager::RequestCommandSignature(TWT::String const& Name, std::vector<TW3DCSCommandArgument> const& CommandArguments, TW3DRootSignature* RootSignature) {
	auto& cs = command_signatures[Name];

	if (!cs) {
		cs = new TW3DCommandSignature(device, CommandArguments, RootSignature);
		TWU::DXSetName(cs->Native, Name);
	}

	return cs;
}

TW3DCommandList* TW3DResourceManager::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type) {
	auto& cl = command_lists[Name];

	if (!cl) {
		cl = CreateCommandList(Type);
		TWU::DXSetName(cl->Native, Name);
	}

	return cl;
}

TW3DCommandList* TW3DResourceManager::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DGraphicsPipeline* InitialState) {
	auto& cl = command_lists[Name];

	if (!cl) {
		cl = CreateCommandList(Type, InitialState);
		TWU::DXSetName(cl->Native, Name);
	}

	return cl;
}

TW3DCommandList* TW3DResourceManager::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DComputePipeline* InitialState) {
	auto& cl = command_lists[Name];

	if (!cl) {
		cl = CreateCommandList(Type, InitialState);
		TWU::DXSetName(cl->Native, Name);
	}

	return cl;
}

TW3DRenderTarget* TW3DResourceManager::RequestRenderTarget(TWT::String const& Name, ID3D12Resource* Buffer) {
	auto& rt = render_targets[Name];

	if (!rt) {
		rt = CreateRenderTarget(Buffer);
		TWU::DXSetName(rt->Native, Name);
	}

	return rt;
}

TW3DRenderTarget* TW3DResourceManager::RequestRenderTarget(TWT::String const& Name, TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	auto& rt = render_targets[Name];

	if (!rt) {
		rt = CreateRenderTarget(Size, Format, ClearValue);
		TWU::DXSetName(rt->Native, Name);
	}

	return rt;
}

TW3DRenderTarget* TW3DResourceManager::RequestRenderTargetCube(TWT::String const& Name, TWT::uint Size, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	auto& rt = render_targets[Name];

	if (!rt) {
		rt = CreateRenderTargetCube(Size, Format, ClearValue);
		TWU::DXSetName(rt->Native, Name);
	}

	return rt;
}

TW3DTexture* TW3DResourceManager::RequestTexture2D(TWT::String const& Name, TWT::uint2 Size, DXGI_FORMAT Format, bool UAV) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateTexture2D(Size, Format, UAV);
		TWU::DXSetName(texture->Native, Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestTexture2D(TWT::String const& Name, const TWT::WString& Filename) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = TW3DTexture::Create2D(device, temp_gcl, srv_descriptor_heap, Filename);
		TWU::DXSetName(texture->Native, Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestTextureArray2D(TWT::String const& Name, TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateTextureArray2D(Size, Depth, Format, UAV);
		TWU::DXSetName(texture->Native, Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestDepthStencilTexture(TWT::String const& Name, TWT::uint2 Size) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateDepthStencilTexture(Size);
		TWU::DXSetName(texture->Native, Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestDepthStencilCubeTexture(TWT::String const& Name, TWT::uint Size, D3D12_RESOURCE_FLAGS Flags, D3D12_DSV_FLAGS DSVFlags) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateDepthStencilCubeTexture(Size, Flags, DSVFlags);
		TWU::DXSetName(texture->Native, Name);
	}

	return texture;
}

TW3DBuffer* TW3DResourceManager::RequestBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes,
	bool UAV, bool UpdateOptimized, D3D12_RESOURCE_STATES InitialState) {

	auto& buffer = buffers[Name];
	if (!buffer)
		buffer = CreateBuffer(Name, ElementCount, ElementSizeInBytes, UAV, UpdateOptimized, InitialState);
	return buffer;
}

TW3DVertexBuffer* TW3DResourceManager::RequestVertexBuffer(TWT::String const& Name, TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes, bool OptimizeForUpdating) {
	auto& buffer = vertex_buffers[Name];

	if (!buffer) {
		buffer = CreateVertexBuffer(VertexCount, SingleVertexSizeInBytes, OptimizeForUpdating);
		TWU::DXSetName(buffer->Native, Name);
	}

	return buffer;
}

TW3DConstantBuffer* TW3DResourceManager::RequestConstantBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes) {
	auto& buffer = constant_buffers[Name];
	if (!buffer)
		buffer = CreateConstantBuffer(Name, ElementCount, ElementSizeInBytes);
	return buffer;
}

TW3DCommandBuffer* TW3DResourceManager::RequestCommandBuffer(TWT::String const& Name, TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize) {
	auto& buffer = command_buffers[Name];

	if (!buffer)
		buffer = CreateCommandBuffer(Name, CommandSignature, MaxCommandCount, SingleCommandByteSize);

	return buffer;
}

TW3DFramebuffer* TW3DResourceManager::RequestFramebuffer(TWT::String const& Name, TWT::uint2 Size) {
	auto& buffer = framebuffers[Name];

	if (!buffer)
		buffer = CreateFramebuffer(Size);

	return buffer;
}

TW3DGraphicsPipeline* TW3DResourceManager::RequestGraphicsPipeline(TWT::String const& Name, TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType,
	DXGI_SAMPLE_DESC SampleDesc, D3D12_RASTERIZER_DESC const& RasterizerState, D3D12_DEPTH_STENCIL_DESC const& DepthStencilState, D3D12_BLEND_DESC const& BlendState,
	std::vector<DXGI_FORMAT> const& OutputFormats, TW3DShader* VertexShader, TW3DShader* GeometryShader, TW3DShader* PixelShader, std::vector<D3D12_INPUT_ELEMENT_DESC> const& InputLayout) {

	auto& gp = graphics_pipelines[Name];

	if (!gp) {
		gp = new TW3DGraphicsPipeline(RootSignature, PrimitiveTopologyType, SampleDesc, RasterizerState, DepthStencilState, BlendState);
		gp->SetInputLayout(InputLayout);
		gp->SetVertexShader(VertexShader);
		gp->SetGeometryShader(GeometryShader);
		gp->SetPixelShader(PixelShader);
		for (TWT::uint i = 0; i < OutputFormats.size(); i++)
			gp->SetRTVFormat(i, OutputFormats[i]);
		gp->Create(device);

		TWU::DXSetName(gp->Native, Name);
	}

	return gp;
}

TW3DGraphicsPipeline* TW3DResourceManager::RequestGraphicsPipeline(TWT::String const& Name, TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType) {
	auto& gp = graphics_pipelines[Name];

	if (!gp) {
		gp = new TW3DGraphicsPipeline(RootSignature, PrimitiveTopologyType);
		TWU::DXSetName(gp->Native, Name);
	}

	return gp;
}

TW3DComputePipeline* TW3DResourceManager::RequestComputePipeline(TWT::String const& Name, TW3DRootSignature* RootSignature, TW3DShader* ComputeShader) {
	auto& cp = compute_pipelines[Name];

	if (!cp) {
		cp = new TW3DComputePipeline(RootSignature);
		cp->SetShader(ComputeShader);
		cp->Create(device);

		TWU::DXSetName(cp->Native, Name);
	}

	return cp;
}

void TW3DResourceManager::ReleaseResource(TW3DResource* Resource) {
	TWT::String name = TWU::DXGetName(Resource->Native);

	if (instanceof(Resource, TW3DRenderTarget))
		ReleaseRenderTarget(name);
	else if (instanceof(Resource, TW3DTexture))
		ReleaseTexture(name);
	else if (instanceof(Resource, TW3DBuffer))
		ReleaseBuffer(name);
	else if (instanceof(Resource, TW3DVertexBuffer))
		ReleaseVertexBuffer(name);
	else if (instanceof(Resource, TW3DConstantBuffer))
		ReleaseConstantBuffer(name);
	else
		TWU::TW3DLogError("Resource \'"s + name + "\' cannot be deleted due to its unknown type!"s);
}

void TW3DResourceManager::ReleaseCommandList(TW3DCommandList* CommandList) {
	ReleaseCommandList(TWU::DXGetName(CommandList->Native));
}

void TW3DResourceManager::ReleaseCommandList(TWT::String const& Name) {
	delete command_lists[Name];
	command_lists.erase(Name);
}

void TW3DResourceManager::ReleaseRenderTarget(TWT::String const& Name) {
	delete render_targets[Name];
	render_targets.erase(Name);
}

void TW3DResourceManager::ReleaseTexture(TWT::String const& Name) {
	delete textures[Name];
	textures.erase(Name);
}

void TW3DResourceManager::ReleaseBuffer(TWT::String const& Name) {
	delete buffers[Name];
	buffers.erase(Name);
}

void TW3DResourceManager::ReleaseVertexBuffer(TWT::String const& Name) {
	delete vertex_buffers[Name];
	vertex_buffers.erase(Name);
}

void TW3DResourceManager::ReleaseConstantBuffer(TWT::String const& Name) {
	delete constant_buffers[Name];
	constant_buffers.erase(Name);
}

void TW3DResourceManager::ReleaseFramebuffer(TWT::String const& Name) {
	delete framebuffers[Name];
	framebuffers.erase(Name);
}

bool TW3DResourceManager::IsCommandListRunning(TW3DCommandList* CommandList) {
	return GetCommandListCommandQueue(CommandList)->IsCommandListRunning(CommandList);
}

void TW3DResourceManager::FlushCommandList(TW3DCommandList* CommandList) {
	GetCommandListCommandQueue(CommandList)->FlushCommandList(CommandList);
}

void TW3DResourceManager::FlushCommandLists() {
	copy_command_queue->FlushCommands();
	compute_command_queue->FlushCommands();
	direct_command_queue->FlushCommands();
}

void TW3DResourceManager::ExecuteCommandList(TW3DCommandList* CommandList) {
	GetCommandListCommandQueue(CommandList)->ExecuteCommandList(CommandList);
}

void TW3DResourceManager::ExecuteCommandLists(const std::vector<TW3DCommandList*>& CommandLists) {
	GetCommandListCommandQueue(CommandLists[0])->ExecuteCommandLists(CommandLists);
}

TW3DDevice* TW3DResourceManager::GetDevice() {
	return device;
}

TW3DDescriptorHeap* TW3DResourceManager::GetSVDescriptorHeap() {
	return srv_descriptor_heap;
}

TW3DCommandQueue* TW3DResourceManager::GetDirectCommandQueue() {
	return direct_command_queue;
}
