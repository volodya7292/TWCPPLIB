#pragma once
#include "TW3DResourceDSV.h"
#include "TW3DResourceRTV.h"
#include "TW3DResourceUAV.h"
#include "TW3DGraphicsPipelineState.h"
#include "TW3DComputePipelineState.h"
#include "TW3DRootSignature.h"

class TW3DResourceSR;
class TW3DResourceVB;
class TW3DResourceCB;
class TW3DResourceManager;
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
	void ResourceBarriers(const TWT::Vector<D3D12_RESOURCE_BARRIER>& barriers);
	void ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	void ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	void CopyBufferRegion(TW3DResource* DstBuffer, TWT::uint64 DstOffset, TW3DResource* SrcBuffer, TWT::uint64 SrcOffset, TWT::uint64 ByteCount);
	void SetPipelineState(TW3DGraphicsPipelineState* PipelineState);
	void SetPipelineState(TW3DComputePipelineState* PipelineState);
	void SetRenderTarget(TW3DResourceRTV* RTV, TW3DResourceDSV* DSV);
	void SetRenderTargets(const TWT::Vector<TW3DResourceRTV*>& RTVs, TW3DResourceDSV* DSV);
	void ClearRTV(TW3DResourceRTV* RTV);
	void ClearRTV(TW3DResourceRTV* RTV, TWT::vec4 Color);
	void ClearDSVDepth(TW3DResourceDSV* DSV, float Depth = 1);
	void SetRootSignature(TW3DRootSignature* RootSignature);
	void SetDescriptorHeap(TW3DDescriptorHeap* heap);
	void SetDescriptorHeaps(TWT::Vector<TW3DDescriptorHeap*> heaps);
	void SetRootDescriptorTable(TWT::uint RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
	void SetRootCBV(TWT::uint RootParameterIndex, TW3DResourceCB* CB, TWT::uint ElementIndex = 0);
	void SetRoot32BitConstant(TWT::uint RootParameterIndex, TWT::uint Data, TWT::uint DestOffsetIn32BitValues);
	void SetRoot32BitConstants(TWT::uint RootParameterIndex, TWT::uint Num32BitValuesToSet, const void* Data, TWT::uint DestOffsetIn32BitValues);
	void SetViewport(const D3D12_VIEWPORT *viewport);
	void SetScissor(const D3D12_RECT* scissor);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);
	void SetVertexBuffer(TWT::uint StartSlot, TW3DResourceVB* view);
	void SetVertexBuffers(TWT::uint StartSlot, const TWT::Vector<D3D12_VERTEX_BUFFER_VIEW>& views);
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view);
	void Draw(TWT::uint VertexCountPerInstance, TWT::uint StartVertexLocation = 0, TWT::uint InstanceCount = 1, TWT::uint StartInstanceLocation = 0);
	void DrawIndexed(TWT::uint IndexCountPerInstance, TWT::uint StartIndexLocation = 0, TWT::uint InstanceCount = 1, TWT::uint StartInstanceLocation = 0, int BaseVertexLocation = 0);
	void Dispatch(TWT::uint ThreadGroupCountX = 1, TWT::uint ThreadGroupCountY = 1, TWT::uint ThreadGroupCountZ = 1);
	void ExecuteIndirect(ID3D12CommandSignature* CommandSignature, TWT::uint MaxCommandCount, ID3D12Resource* ArgumentBuffer,
		TWT::uint64 ArgumentBufferOffset, ID3D12Resource* CountBuffer, TWT::uint64 CountBufferOffset);

	void BindResources(TW3DResourceManager* ResourceManager);
	void BindTexture(TWT::uint RootParameterIndex, TW3DResourceSR* SR);
	void BindRTVTexture(TWT::uint RootParameterIndex, TW3DResourceRTV* RTV);
	void BindUAVBuffer(TWT::uint RootParameterIndex, TW3DResource* UAV);
	void BindUAVTexture(TWT::uint RootParameterIndex, TW3DResourceUAV* UAV);
	void BindUAVSRV(TWT::uint RootParameterIndex, TW3DResourceUAV* UAV);
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