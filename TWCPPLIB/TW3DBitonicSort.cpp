#include "pch.h"
#include "TW3DBitonicSort.h"
#include "BitonicInnerSort.c.h"
#include "BitonicOuterSort.c.h"
#include "BitonicPreSort.c.h"
#include "BitonicSortIndirectArgs.c.h"


TW3D::TW3DBitonicSort::TW3DBitonicSort(TW3DResourceManager* ResourceManager) {
	auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto indirectArgBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cIndirectArgStride * 22 * 23 / 2, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	TW3DDevice* device = ResourceManager->GetDevice();

	m_pDispatchArgs = new TW3DResource(device, &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &indirectArgBufferDesc, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);


	m_pRootSignature = new TW3DRootSignature(false, false, false, false);
	m_pRootSignature->SetParameterConstants(ShaderSpecificConstants, D3D12_SHADER_VISIBILITY_ALL, 0, 2);
	m_pRootSignature->SetParameterUAVBuffer(OutputUAV, D3D12_SHADER_VISIBILITY_ALL, 0);
	m_pRootSignature->SetParameterUAVBuffer(IndexBufferUAV, D3D12_SHADER_VISIBILITY_ALL, 1);
	m_pRootSignature->SetParameterConstants(GenericConstants, D3D12_SHADER_VISIBILITY_ALL, 1, 2);
	m_pRootSignature->Create(device);
	m_pRootSignature->DestroyOnPipelineDestroy = false;

	//auto rootSignatureDesc = CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC(ARRAYSIZE(parameters), parameters);
	//CreateRootSignatureHelper(pDevice, rootSignatureDesc, &m_pRootSignature);

	m_pBitonicIndirectArgsCS = new TW3DComputePipelineState(m_pRootSignature);
	m_pBitonicIndirectArgsCS->SetShader(TW3DCompiledShader(BitonicSortIndirectArgs_ByteCode));
	m_pBitonicIndirectArgsCS->Create(device);

	m_pBitonicInnerSortCS = new TW3DComputePipelineState(m_pRootSignature);
	m_pBitonicInnerSortCS->SetShader(TW3DCompiledShader(BitonicInnerSort_ByteCode));
	m_pBitonicInnerSortCS->Create(device);

	m_pBitonicOuterSortCS = new TW3DComputePipelineState(m_pRootSignature);
	m_pBitonicOuterSortCS->SetShader(TW3DCompiledShader(BitonicOuterSort_ByteCode));
	m_pBitonicOuterSortCS->Create(device);

	m_pBitonicPreSortCS = new TW3DComputePipelineState(m_pRootSignature);
	m_pBitonicPreSortCS->SetShader(TW3DCompiledShader(BitonicPreSort_ByteCode));
	m_pBitonicPreSortCS->Create(device);

	D3D12_INDIRECT_ARGUMENT_DESC indirectArgDesc = {};
	indirectArgDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
	D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc;
	commandSignatureDesc.NodeMask = 0;
	commandSignatureDesc.pArgumentDescs = &indirectArgDesc;
	commandSignatureDesc.ByteStride = sizeof(D3D12_DISPATCH_ARGUMENTS);
	commandSignatureDesc.NumArgumentDescs = 1;

	device->CreateCommandSignature(&commandSignatureDesc, nullptr, &m_pCommandSignature);
}

TW3D::TW3DBitonicSort::~TW3DBitonicSort() {
	delete m_pBitonicIndirectArgsCS;
	delete m_pBitonicInnerSortCS;
	delete m_pBitonicOuterSortCS;
	delete m_pBitonicPreSortCS;
	delete m_pDispatchArgs;
	delete m_pRootSignature;
	TWU::DXSafeRelease(m_pCommandSignature);
}



void TW3D::TW3DBitonicSort::RecordSort(TW3DGraphicsCommandList* CommandList, TW3DResourceUAV* SortKeyBuffer, TW3DResourceUAV* IndexBuffer, TWT::UInt ElementCount, bool SortAscending) {
	if (ElementCount == 0) return;

	const uint32_t AlignedNumElements = TWU::AlignPowerOfTwo(ElementCount);
	const uint32_t MaxIterations = TWU::Log2(std::max(2048u, AlignedNumElements)) - 10;

	CommandList->SetRootSignature(m_pRootSignature);

	struct InputConstants {
		UINT NullIndex;
		UINT ListCount;
	};
	InputConstants constants { SortAscending ? 0xffffffff : 0, ElementCount };
	CommandList->SetRoot32BitConstants(GenericConstants, 2, &constants, 0); // Bugs may occur: put this line after each SetPipelineState

	// Generate execute indirect arguments
	CommandList->SetPipelineState(m_pBitonicIndirectArgsCS);

	CommandList->ResourceBarrier(m_pDispatchArgs, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	CommandList->SetRoot32BitConstant(ShaderSpecificConstants, MaxIterations, 0);
	CommandList->BindUAVBuffer(OutputUAV, m_pDispatchArgs);
	CommandList->BindUAVBuffer(IndexBufferUAV, IndexBuffer);
	CommandList->Dispatch(1, 1, 1);

	// Pre-Sort the buffer up to k = 2048.  This also pads the list with invalid indices
	// that will drift to the end of the sorted list.
	CommandList->ResourceBarrier(m_pDispatchArgs, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
	CommandList->BindUAVBuffer(OutputUAV, SortKeyBuffer);

	auto uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(nullptr);
	//if (!IsPartiallyPreSorted) {
		CommandList->SetPipelineState(m_pBitonicPreSortCS);
		
		CommandList->BindUAVBuffer(OutputUAV, SortKeyBuffer);
		CommandList->BindUAVBuffer(IndexBufferUAV, IndexBuffer);

		CommandList->ExecuteIndirect(m_pCommandSignature, 1, m_pDispatchArgs->Get(), 0, nullptr, 0);
		//CommandList->ResourceBarrier(1, &uavBarrier);
		CommandList->ResourceBarrier(uavBarrier);
	//}

	uint32_t IndirectArgsOffset = cIndirectArgStride;

	// We have already pre-sorted up through k = 2048 when first writing our list, so
	// we continue sorting with k = 4096.  For unnecessarily large values of k, these
	// indirect dispatches will be skipped over with thread counts of 0.

	for (uint32_t k = 4096; k <= AlignedNumElements; k *= 2) {
		CommandList->SetPipelineState(m_pBitonicOuterSortCS);
		CommandList->BindUAVBuffer(OutputUAV, SortKeyBuffer);
		CommandList->BindUAVBuffer(IndexBufferUAV, IndexBuffer);

		for (uint32_t j = k / 2; j >= 2048; j /= 2) {
			struct OuterSortConstants {
				UINT k;
				UINT j;
			} constants { k, j };

			CommandList->SetRoot32BitConstants(ShaderSpecificConstants, 2, &constants, 0);
			CommandList->ExecuteIndirect(m_pCommandSignature, 1, m_pDispatchArgs->Get(), IndirectArgsOffset, nullptr, 0);
			CommandList->ResourceBarrier(uavBarrier);
			IndirectArgsOffset += cIndirectArgStride;
		}

		CommandList->SetPipelineState(m_pBitonicInnerSortCS);
		CommandList->BindUAVBuffer(OutputUAV, SortKeyBuffer);
		CommandList->BindUAVBuffer(IndexBufferUAV, IndexBuffer);
		CommandList->ExecuteIndirect(m_pCommandSignature, 1, m_pDispatchArgs->Get(), IndirectArgsOffset, nullptr, 0);
		CommandList->ResourceBarrier(uavBarrier);
		IndirectArgsOffset += cIndirectArgStride;
	}
}