#include "pch.h"
#include "TW3DScene.h"
#include "TW3DShaders.h"

TW3D::TW3DScene::TW3DScene(TW3DResourceManager* ResourceManager) :
	resource_manager(ResourceManager)
{
	Camera = new TW3DPerspectiveCamera(ResourceManager);

	gvb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(TWT::DefaultVertex));
	gnb = ResourceManager->CreateUnorderedAccessView(1024, sizeof(TWT::LBVHNode));
}

TW3D::TW3DScene::~TW3DScene() {
	delete Camera;
	delete gvb;
	delete gnb;
}

void TW3D::TW3DScene::AddObject(TW3DObject* Object) {
	Objects.push_back(Object);
}

void TW3D::TW3DScene::RecordBeforeExecution() {
	// Collect vertex buffers & meshes
	// -------------------------------------------------------------------------------------------------------------------------
	TWT::UInt VertexOffset = 0;
	TWT::UInt NodeOffset = 0;
	vertex_buffers.clear();
	vertex_meshes.clear();
	TWT::Vector<TW3DVertexBuffer*> buffers;
	TWT::Vector<TW3DVertexMesh*> meshes;
	for (TW3DObject* object : Objects) {
		TW3DVertexMesh* mesh = object->VMInstance.VertexMesh;

		if (std::find(meshes.begin(), meshes.end(), mesh) == meshes.end()) {
			meshes.push_back(mesh);
			vertex_meshes.push_back(std::pair(mesh, std::pair(VertexOffset, NodeOffset)));
			NodeOffset += mesh->LBVH->GetNodeCount();
		}

		for (TW3DVertexBuffer* vb : object->VMInstance.VertexMesh->VertexBuffers)
			if (std::find(buffers.begin(), buffers.end(), vb) == buffers.end()) {
				buffers.push_back(vb);
				vertex_buffers.push_back(std::pair(vb, VertexOffset));
				VertexOffset += vb->GetVertexCount();
			}
	}
	gvb_vertex_count = VertexOffset + 1;
	gnb_node_count = NodeOffset + 1;


	auto gcl = resource_manager->GetTemporaryDirectCommandList();

	// Build Global Vertex Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	gcl->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	gcl->SetPipelineState(TW3DShaders::GetGraphicsShader(TW3DShaders::BuildGVB));
	gcl->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gcl->BindUAVBuffer(0, gvb);
	for (auto entry : vertex_buffers) {
		gcl->SetVertexBuffer(0, entry.first->GetResource());
		gcl->SetRoot32BitConstant(1, entry.second, 0);
		gcl->Draw(entry.first->GetVertexCount());
	}
	gcl->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	gcl->Close();
	resource_manager->ExecuteCommandList(gcl);
	resource_manager->FlushCommandList(gcl);

	// Build LBVHs
	// -------------------------------------------------------------------------------------------------------------------------
	for (auto entry : vertex_meshes)
		entry.first->LBVH->BuildFromPrimitives(gvb, entry.second.first);


	auto cl = resource_manager->GetTemporaryComputeCommandList();

	// Build Global Node Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	cl->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cl->SetPipelineState(TW3DShaders::GetComputeShader(TW3DShaders::BuildGLBVHNB));
	cl->BindUAVBuffer(0, gnb);
	for (auto entry : vertex_meshes) {
		cl->BindUAVBufferSRV(1, entry.first->LBVH->GetNodeBuffer());
		cl->SetRoot32BitConstant(2, entry.second.second, 0);
		cl->Dispatch(entry.first->LBVH->GetNodeCount());
	}
	cl->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	cl->Close();

	resource_manager->ExecuteCommandList(cl);
	resource_manager->FlushCommandList(cl);
}
