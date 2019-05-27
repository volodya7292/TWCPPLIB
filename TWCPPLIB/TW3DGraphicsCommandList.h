#pragma once
#include "TW3DGraphicsPipelineState.h"
#include "TW3DComputePipelineState.h"
#include "TW3DRootSignature.h"

class TW3DResource;
class TW3DRenderTarget;
class TW3DResourceUAV;
class TW3DTexture;
class TW3DVertexBuffer;
class TW3DConstantBuffer;
class TW3DResourceManager;
class TW3DDescriptorHeap;
class TW3DObject;
class TW3DPerspectiveCamera;

class TW3DGraphicsCommandList {
public:
	TW3DGraphicsCommandList(TW3DDevice* Device, D3D12_COMMAND_LIST_TYPE Type);
	~TW3DGraphicsCommandList();

	ID3D12GraphicsCommandList* Get();
	D3D12_COMMAND_LIST_TYPE GetType();

	void UpdateSubresources(TW3DResource* DestinationResource, TW3DResource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
		TWT::uint SubresourcesCount = 1, TWT::uint64 IntermediateOffset = 0, TWT::uint FirstSubresource = 0);
	void UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
		TWT::uint SubresourcesCount = 1, TWT::uint64 IntermediateOffset = 0, TWT::uint FirstSubresource = 0);

	void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);
	void ResourceBarriers(const std::vector<D3D12_RESOURCE_BARRIER>& barriers);
	void ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	void ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	void CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount);
	void SetPipelineState(TW3DGraphicsPipelineState* PipelineState);
	void SetPipelineState(TW3DComputePipelineState* PipelineState);
	void SetRenderTarget(TW3DRenderTarget* RenderTarget, TW3DTexture* DSV);
	void SetRenderTargets(const std::vector<TW3DRenderTarget*>& RTVs, TW3DTexture* DSV);
	void ClearRTV(TW3DRenderTarget* RenderTarget);
	void ClearRTV(TW3DRenderTarget* RenderTarget, TWT::vec4 Color);
	void ClearDSVDepth(TW3DTexture* Texture);
	void SetRootSignature(TW3DRootSignature* RootSignature);
	void SetDescriptorHeap(TW3DDescriptorHeap* heap);
	void SetDescriptorHeaps(std::vector<TW3DDescriptorHeap*> heaps);
	void SetRootDescriptorTable(TWT::uint RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
	void SetViewport(const D3D12_VIEWPORT *viewport);
	void SetScissor(const D3D12_RECT* scissor);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);
	void SetVertexBuffer(TWT::uint StartSlot, TW3DVertexBuffer* VertexBuffer);
	void SetVertexBuffers(TWT::uint StartSlot, const std::vector<D3D12_VERTEX_BUFFER_VIEW>& views);
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view);
	void Draw(TWT::uint VertexCountPerInstance, TWT::uint StartVertexLocation = 0, TWT::uint InstanceCount = 1, TWT::uint StartInstanceLocation = 0);
	void DrawIndexed(TWT::uint IndexCountPerInstance, TWT::uint StartIndexLocation = 0, TWT::uint InstanceCount = 1, TWT::uint StartInstanceLocation = 0, int BaseVertexLocation = 0);
	void Dispatch(TWT::uint ThreadGroupCountX = 1, TWT::uint ThreadGroupCountY = 1, TWT::uint ThreadGroupCountZ = 1);
	void ExecuteIndirect(ID3D12CommandSignature* CommandSignature, TWT::uint MaxCommandCount, ID3D12Resource* ArgumentBuffer,
		TWT::uint64 ArgumentBufferOffset, ID3D12Resource* CountBuffer, TWT::uint64 CountBufferOffset);

	void BindResources(TW3DResourceManager* ResourceManager);
	void BindBuffer(TWT::uint RootParameterIndex, TW3DResource* Resource, bool UAV = false);
	void BindTexture(TWT::uint RootParameterIndex, TW3DTexture* Texture, bool UAV = false);
	void BindTexture(TWT::uint RootParameterIndex, TW3DRenderTarget* RenderTarget);
	void BindConstantBuffer(TWT::uint RootParameterIndex, TW3DConstantBuffer* CB, TWT::uint ElementIndex = 0);
	void Bind32BitConstant(TWT::uint RootParameterIndex, TWT::uint Data, TWT::uint DestOffsetIn32BitValues);
	void Bind32BitConstants(TWT::uint RootParameterIndex, TWT::uint Num32BitValuesToSet, const void* Data, TWT::uint DestOffsetIn32BitValues);
	void BindCameraCBV(TWT::uint RootParameterIndex, TW3DPerspectiveCamera* Camera);
	void DrawObject(TW3DObject* object, TWT::uint ModelCBRootParameterIndex);

	void Reset();
	void Close();
	void EmptyReset();
	bool IsEmpty();

	static TW3DGraphicsCommandList* CreateDirect(TW3DDevice* device);
	static TW3DGraphicsCommandList* CreateBundle(TW3DDevice* device);
	static TW3DGraphicsCommandList* CreateCompute(TW3DDevice* device);
	static TW3DGraphicsCommandList* CreateCopy(TW3DDevice* device);

	TWT::uint64 SignalValue = 0;

private:
	ID3D12GraphicsCommandList* command_list;
	ID3D12CommandAllocator* command_allocator;

	bool empty = true;

	const D3D12_COMMAND_LIST_TYPE type;
};

D3D12_RESOURCE_BARRIER TW3DUAVBarrier(TW3DResource* Resource = nullptr);