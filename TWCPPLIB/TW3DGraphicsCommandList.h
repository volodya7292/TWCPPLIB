#pragma once
#include "TW3DDevice.h"
#include "TW3DDescriptorHeap.h"

namespace TW3D {
	class TW3DGraphicsCommandList {
	public:
		TW3DGraphicsCommandList(TW3DDevice* device, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* commandAllocator);
		~TW3DGraphicsCommandList();

		ID3D12GraphicsCommandList* Get();

		void UpdateSubresources(ID3D12Resource* DestinationResource, ID3D12Resource* Intermediate, D3D12_SUBRESOURCE_DATA* SrcData,
			TWT::UInt SubresourcesCount = 1, TWT::UInt64 IntermediateOffset = 0, TWT::UInt FirstSubresource = 0);

		void ResourceBarrier(const D3D12_RESOURCE_BARRIER barrier);
		void ResourceBarriers(const TWT::Vector<D3D12_RESOURCE_BARRIER>& barriers);
		void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE* RTVDescriptor, const D3D12_CPU_DESCRIPTOR_HANDLE* DSVDescriptor);
		void SetRenderTargets(TWT::UInt RTVDescriptorCount, const D3D12_CPU_DESCRIPTOR_HANDLE* RTVDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE* DSVDescriptor);
		void ClearRTV(D3D12_CPU_DESCRIPTOR_HANDLE RTVDescriptor, const TWT::Float (&RGBA)[4]);
		void ClearDSVDepth(D3D12_CPU_DESCRIPTOR_HANDLE DSVDescriptor, TWT::Float Depth);
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

		void Reset(ID3D12CommandAllocator *commandAllocator, ID3D12PipelineState *initialState);
		void Close();

		static TW3DGraphicsCommandList* CreateDirect(TW3DDevice* device, ID3D12CommandAllocator* commandAllocator);

	private:
		ID3D12GraphicsCommandList* commandList;
	};
}