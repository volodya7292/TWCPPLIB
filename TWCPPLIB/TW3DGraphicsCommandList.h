#pragma once
#include "TW3DResourceDSV.h"
#include "TW3DResourceRTV.h"
#include "TW3DResourceUAV.h"
#include "TW3DGraphicsPipelineState.h"
#include "TW3DComputePipelineState.h"
#include "TW3DRootSignature.h"

namespace TW3D {
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

		void UpdateSubresources(TW3DResource* DestinationResource, TW3DResource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
			TWT::UInt SubresourcesCount = 1, TWT::UInt64 IntermediateOffset = 0, TWT::UInt FirstSubresource = 0);
		void UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
			TWT::UInt SubresourcesCount = 1, TWT::UInt64 IntermediateOffset = 0, TWT::UInt FirstSubresource = 0);

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER barrier);
		void ResourceBarriers(const TWT::Vector<D3D12_RESOURCE_BARRIER>& barriers);
		void ResourceBarrier(TW3DResource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
		void ResourceBarrier(ID3D12Resource* Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
		void SetPipelineState(TW3DGraphicsPipelineState* PipelineState);
		void SetPipelineState(TW3DComputePipelineState* PipelineState);
		void SetRenderTarget(TW3DResourceRTV* RTV, TW3DResourceDSV* DSV);
		void SetRenderTargets(const TWT::Vector<TW3DResourceRTV*>& RTVs, TW3DResourceDSV* DSV);
		void ClearRTV(TW3DResourceRTV* RTV);
		void ClearDSVDepth(TW3DResourceDSV* DSV, TWT::Float Depth = 1);
		void SetRootSignature(TW3DRootSignature* RootSignature);
		void SetDescriptorHeap(TW3DDescriptorHeap* heap);
		void SetDescriptorHeaps(TWT::Vector<TW3DDescriptorHeap*> heaps);
		void SetRootDescriptorTable(TWT::UInt RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
		void SetRootCBV(TWT::UInt RootParameterIndex, TW3DResourceCB* CB, TWT::UInt ElementIndex = 0);
		void SetRoot32BitConstant(TWT::UInt RootParameterIndex, TWT::UInt Data, TWT::UInt DestOffsetIn32BitValues);
		void SetRoot32BitConstants(TWT::UInt RootParameterIndex, TWT::UInt Num32BitValuesToSet, const void* Data, TWT::UInt DestOffsetIn32BitValues);
		void SetViewport(const D3D12_VIEWPORT *viewport);
		void SetScissor(const D3D12_RECT* scissor);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);
		void SetVertexBuffer(TWT::UInt StartSlot, TW3DResourceVB* view);
		void SetVertexBuffers(TWT::UInt StartSlot, TWT::Vector<D3D12_VERTEX_BUFFER_VIEW> views);
		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view);
		void Draw(TWT::UInt VertexCountPerInstance, TWT::UInt StartVertexLocation = 0, TWT::UInt InstanceCount = 1, TWT::UInt StartInstanceLocation = 0);
		void DrawIndexed(TWT::UInt IndexCountPerInstance, TWT::UInt StartIndexLocation = 0, TWT::UInt InstanceCount = 1, TWT::UInt StartInstanceLocation = 0, TWT::Int BaseVertexLocation = 0);
		void Dispatch(TWT::UInt ThreadGroupCountX = 1, TWT::UInt ThreadGroupCountY = 1, TWT::UInt ThreadGroupCountZ = 1);
		void ExecuteIndirect(ID3D12CommandSignature* CommandSignature, TWT::UInt MaxCommandCount, ID3D12Resource* ArgumentBuffer,
			TWT::UInt64 ArgumentBufferOffset, ID3D12Resource* CountBuffer, TWT::UInt64 CountBufferOffset);

		void BindResources(TW3DResourceManager* ResourceManager);
		void BindTexture(TWT::UInt RootParameterIndex, TW3DResourceSR* SR);
		void BindRTVTexture(TWT::UInt RootParameterIndex, TW3DResourceRTV* RTV);
		void BindUAVBuffer(TWT::UInt RootParameterIndex, TW3DResource* UAV);
		void BindUAVBufferSRV(TWT::UInt RootParameterIndex, TW3DResourceUAV* UAV);
		void BindUAVTexture(TWT::UInt RootParameterIndex, TW3DResourceUAV* UAV);
		void BindUAVTextureSRV(TWT::UInt RootParameterIndex, TW3DResourceUAV* UAV);
		void BindCameraCBV(TWT::UInt RootParameterIndex, TW3DPerspectiveCamera* Camera);
		void DrawObject(TW3DObject* object, TWT::UInt ModelCBRootParameterIndex);

		void Reset();
		void Close();
		void EmptyReset();
		TWT::Bool IsEmpty();

		static TW3DGraphicsCommandList* CreateDirect(TW3DDevice* device);
		static TW3DGraphicsCommandList* CreateBundle(TW3DDevice* device);
		static TW3DGraphicsCommandList* CreateCompute(TW3DDevice* device);

		const D3D12_COMMAND_LIST_TYPE Type;

		TWT::UInt64 SignalValue = 0;

	private:
		ID3D12GraphicsCommandList* command_list;
		ID3D12CommandAllocator* command_allocator;

		TWT::Bool empty = true;
	};
}