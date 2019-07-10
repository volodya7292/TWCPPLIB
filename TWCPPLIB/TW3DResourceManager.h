#pragma once
#include "TW3DCommandQueue.h"
#include "TW3DConstantBuffer.h"
#include "TW3DTexture.h"
#include "TW3DBuffer.h"
#include "TW3DRenderTarget.h"
#include "TW3DVertexBuffer.h"
#include "TW3DFramebuffer.h"
#include "TW3DCommandBuffer.h"
#include "TW3DTypes.h"

enum TW3DCommandListType {
	TW3D_CL_DIRECT,
	TW3D_CL_COMPUTE,
	TW3D_CL_COPY,
	TW3D_CL_BUNDLE
};

class TW3DResourceManager {
public:
	TW3DResourceManager(TW3DDevice* Device);
	~TW3DResourceManager();

	TW3DFramebuffer*    CreateFramebuffer(TWT::String Name, TWT::uint2 Size, TW3DFramebufferType Type = TW3D_FRAMEBUFFER_DEFAULT);
	TW3DCommandBuffer*  CreateCommandBuffer(TWT::String const& Name, TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize);
	TW3DRenderTarget*   CreateRenderTarget(TWT::String Name, ID3D12Resource* Buffer);
	TW3DRenderTarget*   CreateRenderTarget(TWT::String Name, TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	TW3DRenderTarget*   CreateRenderTargetCube(TWT::String Name, TWT::uint Size, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	TW3DBuffer*         CreateBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes,
		bool UAV = false, bool UpdateOptimized = false, D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	TW3DVertexBuffer*   CreateVertexBuffer(TWT::String Name, TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex), bool OptimizeForUpdating = false);
	TW3DConstantBuffer* CreateConstantBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes);
	TW3DTexture*        CreateDepthStencilTexture(TWT::String Name, TWT::uint2 Size);
	TW3DTexture*        CreateDepthStencilCubeTexture(TWT::String Name, TWT::uint Size, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE, D3D12_DSV_FLAGS DSVFlags = D3D12_DSV_FLAG_NONE);
	TW3DTexture*        CreateTexture2D(const TWT::WString& Filename);
	TW3DTexture*        CreateTexture2D(TWT::String Name, TWT::uint2 Size, DXGI_FORMAT Format, bool UAV = false);
	TW3DTexture*        CreateTextureArray2D(TWT::String Name, TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV = false);
	TW3DCommandList*    CreateCommandList(TW3DCommandListType Type);
	TW3DCommandList*    CreateCommandList(TW3DCommandListType Type, TW3DGraphicsPipeline* InitialState);
	TW3DCommandList*    CreateCommandList(TW3DCommandListType Type, TW3DComputePipeline* InitialState);

	TW3DCommandList*  GetTemporaryDirectCommandList();
	TW3DCommandList*  GetTemporaryComputeCommandList();
	TW3DCommandList*  GetTemporaryCopyCommandList();
	TW3DCommandQueue* GetCommandListCommandQueue(TW3DCommandList* CommandList);

	TW3DShader*           RequestShader(TWT::String const& Name, D3D12_SHADER_BYTECODE const& ByteCode);
	TW3DRootSignature*    RequestRootSignature(TWT::String const& Name, std::vector<TW3DRSRootParameter> const& RootParameters, bool VS = true, bool PS = true, bool GS = false, bool IA = true);
	TW3DRootSignature*    RequestRootSignature(TWT::String const& Name, std::vector<TW3DRSRootParameter> const& RootParameters,
		std::vector<D3D12_STATIC_SAMPLER_DESC> StaticSamplers, bool VS = true, bool PS = true, bool GS = false, bool IA = true);
	TW3DCommandSignature* RequestCommandSignature(TWT::String const& Name, std::vector<TW3DCSCommandArgument> const& CommandArguments, TW3DRootSignature* RootSignature = nullptr);
	TW3DCommandList*      RequestCommandList(TWT::String const& Name, TW3DCommandListType Type);
	TW3DCommandList*      RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DGraphicsPipeline* InitialState);
	TW3DCommandList*      RequestCommandList(TWT::String const& Name, TW3DCommandListType Type, TW3DComputePipeline* InitialState);
	TW3DRenderTarget*     RequestRenderTarget(TWT::String const& Name, ID3D12Resource* Buffer);
	TW3DRenderTarget*     RequestRenderTarget(TWT::String const& Name, TWT::uint2 Size, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	TW3DRenderTarget*     RequestRenderTargetCube(TWT::String const& Name, TWT::uint Size, DXGI_FORMAT Format, TWT::float4 ClearValue = TWT::float4(-1));
	TW3DTexture*          RequestTexture2D(TWT::String const& Name, TWT::uint2 Size, DXGI_FORMAT Format, bool UAV = false);
	TW3DTexture*          RequestTexture2D(TWT::String const& Name, const TWT::WString& Filename);
	TW3DTexture*          RequestTextureArray2D(TWT::String const& Name, TWT::uint2 Size, TWT::uint Depth, DXGI_FORMAT Format, bool UAV = false);
	TW3DTexture*          RequestDepthStencilTexture(TWT::String const& Name, TWT::uint2 Size);
	TW3DTexture*          RequestDepthStencilCubeTexture(TWT::String const& Name, TWT::uint Size, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE, D3D12_DSV_FLAGS DSVFlags = D3D12_DSV_FLAG_NONE);
	TW3DBuffer*           RequestBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes,
		bool UAV = false, bool UpdateOptimized = false, D3D12_RESOURCE_STATES InitialState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	TW3DVertexBuffer*     RequestVertexBuffer(TWT::String const& Name, TWT::uint VertexCount, TWT::uint SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex), bool OptimizeForUpdating = false);
	TW3DConstantBuffer*   RequestConstantBuffer(TWT::String const& Name, TWT::uint ElementCount, TWT::uint ElementSizeInBytes);
	TW3DCommandBuffer*    RequestCommandBuffer(TWT::String const& Name, TW3DCommandSignature* CommandSignature, TWT::uint MaxCommandCount, TWT::uint SingleCommandByteSize);
	TW3DFramebuffer*      RequestFramebuffer(TWT::String const& Name, TWT::uint2 Size, TW3DFramebufferType Type = TW3D_FRAMEBUFFER_DEFAULT);
	TW3DGraphicsPipeline* RequestGraphicsPipeline(TWT::String const& Name, TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType,
		DXGI_SAMPLE_DESC SampleDesc, D3D12_RASTERIZER_DESC const& RasterizerState, D3D12_DEPTH_STENCIL_DESC const& DepthStencilState, D3D12_BLEND_DESC const& BlendState,
		std::vector<DXGI_FORMAT> const& OutputFormats, TW3DShader* VertexShader, TW3DShader* PixelShader, std::vector<D3D12_INPUT_ELEMENT_DESC> const& InputLayout);
	TW3DGraphicsPipeline* RequestGraphicsPipeline(TWT::String const& Name, TW3DRootSignature* RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType,
		DXGI_SAMPLE_DESC SampleDesc, D3D12_RASTERIZER_DESC const& RasterizerState, D3D12_DEPTH_STENCIL_DESC const& DepthStencilState, D3D12_BLEND_DESC const& BlendState,
		std::vector<DXGI_FORMAT> const& OutputFormats, TW3DShader* VertexShader, TW3DShader* GeometryShader, TW3DShader* PixelShader, std::vector<D3D12_INPUT_ELEMENT_DESC> const& InputLayout);
	TW3DGraphicsPipeline* RequestGraphicsPipeline(TWT::String const& Name, TW3DRootSignature* RootSignature,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	TW3DComputePipeline*  RequestComputePipeline(TWT::String const& Name, TW3DRootSignature* RootSignature, TW3DShader* ComputeShader);

	void ReleaseCommandList(TWT::String const& Name);
	void ReleaseRenderTarget(TWT::String const& Name);
	void ReleaseTexture(TWT::String const& Name);
	void ReleaseBuffer(TWT::String const& Name);
	void ReleaseVertexBuffer(TWT::String const& Name);
	void ReleaseConstantBuffer(TWT::String const& Name);
	void ReleaseFramebuffer(TWT::String const& Name);


	bool IsCommandListRunning(TW3DCommandList* CommandList);
	void FlushCommandList(TW3DCommandList* CommandList);
	void FlushCommandLists();
	void ExecuteCommandList(TW3DCommandList* CommandList);
	// Command lists must be of the same type
	void ExecuteCommandLists(const std::vector<TW3DCommandList*>& CommandLists);

	TW3DDevice* GetDevice();
	TW3DDescriptorHeap* GetSVDescriptorHeap();
	TW3DCommandQueue* GetDirectCommandQueue();

private:
	TW3DDevice* device;
	TW3DTempGCL* temp_gcl;
	TW3DCommandList* temp_direct_cl;
	TW3DCommandList* temp_compute_cl;
	TW3DCommandList* temp_copy_cl;
	TW3DDescriptorHeap* rtv_descriptor_heap;
	TW3DDescriptorHeap* dsv_descriptor_heap;
	TW3DDescriptorHeap* srv_descriptor_heap;
	TW3DDescriptorHeap* uav_descriptor_heap;
	TW3DCommandQueue* direct_command_queue;
	TW3DCommandQueue* compute_command_queue;
	TW3DCommandQueue* copy_command_queue;

	std::unordered_map<TWT::String, TW3DShader*>              shaders;
	std::unordered_map<TWT::String, TW3DRootSignature*>       root_signatures;
	std::unordered_map<TWT::String, TW3DCommandSignature*>    command_signatures;
	std::unordered_map<TWT::String, TW3DCommandList*>         command_lists;
	std::unordered_map<TWT::String, TW3DRenderTarget*>        render_targets;
	std::unordered_map<TWT::String, TW3DTexture*>             textures;
	std::unordered_map<TWT::String, TW3DBuffer*>              buffers;
	std::unordered_map<TWT::String, TW3DVertexBuffer*>        vertex_buffers;
	std::unordered_map<TWT::String, TW3DConstantBuffer*>      constant_buffers;
	std::unordered_map<TWT::String, TW3DCommandBuffer*>       command_buffers;
	std::unordered_map<TWT::String, TW3DFramebuffer*>         framebuffers;
	std::unordered_map<TWT::String, TW3DGraphicsPipeline*>    graphics_pipelines;
	std::unordered_map<TWT::String, TW3DComputePipeline*>     compute_pipelines;
};