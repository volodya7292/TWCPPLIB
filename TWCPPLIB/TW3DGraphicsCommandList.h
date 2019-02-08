#pragma once
#include "TW3DResourceDSV.h"
#include "TW3DResourceRTV.h"
#include "TW3DPipelineState.h"

namespace TW3D {
	class TW3DGraphicsCommandList {
	public:
		TW3DGraphicsCommandList(TW3DDevice* device, D3D12_COMMAND_LIST_TYPE type);
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
		void SetPipelineState(TW3DPipelineState* PipelineState);
		void SetRenderTarget(TW3DResourceRTV* RTV, TW3DResourceDSV* DSV);
		void SetRenderTargets(TWT::UInt RTVDescriptorCount, const D3D12_CPU_DESCRIPTOR_HANDLE* RTVDescriptors, TW3DResourceDSV* DSV);
		void ClearRTV(TW3DResourceRTV* RTV, const TWT::Float (&RGBA)[4]);
		void ClearDSVDepth(TW3DResourceDSV* DSV, TWT::Float Depth);
		void SetGraphicsRootSignature(ID3D12RootSignature* RootSignature);
		void SetDescriptorHeap(TW3DDescriptorHeap* heap);
		void SetDescriptorHeaps(TWT::Vector<TW3DDescriptorHeap*> heaps);
		void SetGraphicsRootDescriptorTable(TWT::UInt RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor);
		void SetGraphicsRootCBV(TWT::UInt RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation);
		void SetViewport(const D3D12_VIEWPORT *viewport);
		void SetScissor(const D3D12_RECT* scissor);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);
		void SetVertexBuffer(TWT::UInt StartSlot, const D3D12_VERTEX_BUFFER_VIEW* view);
		void SetVertexBuffers(TWT::UInt StartSlot, TWT::Vector<D3D12_VERTEX_BUFFER_VIEW> views);
		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view);
		void Draw(TWT::UInt VertexCountPerInstance, TWT::UInt StartVertexLocation = 0, TWT::UInt InstanceCount = 1, TWT::UInt StartInstanceLocation = 0);
		void DrawIndexed(TWT::UInt IndexCountPerInstance, TWT::UInt StartIndexLocation = 0, TWT::UInt InstanceCount = 1, TWT::UInt StartInstanceLocation = 0, TWT::Int BaseVertexLocation = 0);

		void Reset();
		void Close();

		static TW3DGraphicsCommandList* CreateDirect(TW3DDevice* device);

	private:
		ID3D12GraphicsCommandList* commandList;
		ID3D12CommandAllocator* CommandAllocator;
	};
}