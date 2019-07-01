#include "pch.h"
#include "TW3DResourceManager.h"

TW3DResourceManager::TW3DResourceManager(TW3DDevice* Device) :
	device(Device)
{
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
	for (auto [name, f_buf] : framebuffers)
		delete f_buf;

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

TW3DBuffer* TW3DResourceManager::CreateBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes, bool UAV) {
	auto* buffer = new TW3DBuffer(device, temp_gcl, false, ElementSizeInBytes, UAV, srv_descriptor_heap);
	buffer->Create(ElementCount);
	return buffer;
}

TW3DVertexBuffer* TW3DResourceManager::CreateVertexBuffer(TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes, bool OptimizeForUpdating) {
	return new TW3DVertexBuffer(device, temp_gcl, OptimizeForUpdating, VertexCount, SingleVertexSizeInBytes);
}

TW3DConstantBuffer* TW3DResourceManager::CreateConstantBuffer(TWT::uint ElementCount, TWT::uint ElementSizeInBytes) {
	return new TW3DConstantBuffer(device, ElementCount, ElementSizeInBytes);
}

TW3DTexture* TW3DResourceManager::CreateDepthStencilTexture(TWT::uint2 Size) {
	auto* texture = new TW3DTexture(device, temp_gcl, DXGI_FORMAT_D32_FLOAT, nullptr, nullptr, dsv_descriptor_heap);
	texture->CreateDepthStencil(Size);
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

TW3DCommandList* TW3DResourceManager::CreateCommandList(TW3DCommandListType Type, TW3DGraphicsPipelineState* InitialState) {
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

TW3DCommandList* TW3DResourceManager::CreateCommandList(TW3DCommandListType Type, TW3DComputePipelineState* InitialState) {
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

TW3DCommandList* TW3DResourceManager::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type) {
	auto& cl = command_lists[Name];

	if (!cl) {
		cl = CreateCommandList(Type);
		TWU::DXSetName(cl->Get(), Name);
	}

	return cl;
}

TW3DCommandList* TW3DResourceManager::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DGraphicsPipelineState* InitialState) {
	auto& cl = command_lists[Name];

	if (!cl) {
		cl = CreateCommandList(Type, InitialState);
		TWU::DXSetName(cl->Get(), Name);
	}

	return cl;
}

TW3DCommandList* TW3DResourceManager::RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DComputePipelineState* InitialState) {
	auto& cl = command_lists[Name];

	if (!cl) {
		cl = CreateCommandList(Type, InitialState);
		TWU::DXSetName(cl->Get(), Name);
	}

	return cl;
}

TW3DRenderTarget* TW3DResourceManager::RequestRenderTarget(TWT::String const& Name, ID3D12Resource* Buffer) {
	auto& rt = render_targets[Name];

	if (!rt) {
		rt = CreateRenderTarget(Buffer);
		TWU::DXSetName(rt->Get(), Name);
	}

	return rt;
}

TW3DRenderTarget* TW3DResourceManager::RequestRenderTarget(TWT::String const& Name, TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue) {
	auto& rt = render_targets[Name];

	if (!rt) {
		rt = CreateRenderTarget(Size, Format, ClearValue);
		TWU::DXSetName(rt->Get(), Name);
	}

	return rt;
}

TW3DTexture* TW3DResourceManager::RequestTexture2D(TWT::String const& Name, TWT::uint2 Size, DXGI_FORMAT Format, bool UAV) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateTexture2D(Size, Format, UAV);
		TWU::DXSetName(texture->Get(), Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestTexture2D(TWT::String const& Name, const TWT::WString& Filename) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = TW3DTexture::Create2D(device, temp_gcl, srv_descriptor_heap, Filename);
		TWU::DXSetName(texture->Get(), Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestTextureArray2D(TWT::String const& Name, TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateTextureArray2D(Size, Depth, Format, UAV);
		TWU::DXSetName(texture->Get(), Name);
	}

	return texture;
}

TW3DTexture* TW3DResourceManager::RequestDepthStencilTexture(TWT::String const& Name, TWT::uint2 Size) {
	auto& texture = textures[Name];

	if (!texture) {
		texture = CreateDepthStencilTexture(Size);
		TWU::DXSetName(texture->Get(), Name);
	}

	return texture;
}

TW3DBuffer* TW3DResourceManager::RequestBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes, bool UAV) {
	auto& buffer = buffers[Name];

	if (!buffer) {
		buffer = CreateBuffer(ElementCount, ElementSizeInBytes, UAV);
		TWU::DXSetName(buffer->Get(), Name);
	}

	return buffer;
}

TW3DVertexBuffer* TW3DResourceManager::RequestVertexBuffer(TWT::String const& Name, TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes, bool OptimizeForUpdating) {
	auto& buffer = vertex_buffers[Name];

	if (!buffer) {
		buffer = CreateVertexBuffer(VertexCount, SingleVertexSizeInBytes, OptimizeForUpdating);
		TWU::DXSetName(buffer->Get(), Name);
	}

	return buffer;
}

TW3DConstantBuffer* TW3DResourceManager::RequestConstantBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes) {
	auto& buffer = constant_buffers[Name];

	if (!buffer) {
		buffer = CreateConstantBuffer(ElementCount, ElementSizeInBytes);
		TWU::DXSetName(buffer->Get(), Name);
	}

	return buffer;
}

TW3DFramebuffer* TW3DResourceManager::RequestFramebuffer(TWT::String const& Name, TWT::uint2 Size) {
	auto& buffer = framebuffers[Name];

	if (!buffer)
		buffer = new TW3DFramebuffer(this, Size);

	return buffer;
}

void TW3DResourceManager::ReleaseCommandList(TWT::String const& Name) {
	delete command_lists.at(Name);
	command_lists.erase(Name);
}

void TW3DResourceManager::ReleaseRenderTarget(TWT::String const& Name) {
	delete render_targets.at(Name);
	render_targets.erase(Name);
}

void TW3DResourceManager::ReleaseTexture(TWT::String const& Name) {
	delete textures.at(Name);
	textures.erase(Name);
}

void TW3DResourceManager::ReleaseBuffer(TWT::String const& Name) {
	delete buffers.at(Name);
	buffers.erase(Name);
}

void TW3DResourceManager::ReleaseVertexBuffer(TWT::String const& Name) {
	delete vertex_buffers.at(Name);
	vertex_buffers.erase(Name);
}

void TW3DResourceManager::ReleaseConstantBuffer(TWT::String const& Name) {
	delete constant_buffers.at(Name);
	constant_buffers.erase(Name);
}

void TW3DResourceManager::ReleaseFramebuffer(TWT::String const& Name) {
	delete framebuffers.at(Name);
	framebuffers.erase(Name);
}

void TW3DResourceManager::ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter) {
	temp_gcl->Reset();
	temp_gcl->ResourceBarrier(Resource->Get(), StateBefore, StateAfter);
	temp_gcl->Execute();
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
