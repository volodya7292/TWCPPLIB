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
	objects.push_back(Object);
}

void TW3D::TW3DScene::RecordBeforeExecution() {
	auto gcl = resource_manager->GetTemporaryDirectCommandList();
	gcl->Reset();

	// Build Global Vertex Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	gcl->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	gcl->SetPipelineState(TW3DShaders::GetGraphicsShader(TW3DShaders::BuildGVB));
	gcl->BindUAVBuffer(0, gvb);
	TWT::UInt VertexOffset = 0;
	gvb_vertex_buffers.clear();
	for (TW3DObject* object : objects)
		for (TW3DVertexBuffer* vb : object->VMInstance.VertexMesh->VertexBuffers)
			if (std::find(gvb_vertex_buffers.begin(), gvb_vertex_buffers.end(), vb) == gvb_vertex_buffers.end()) {
				gvb_vertex_buffers.push_back(std::pair(vb, VertexOffset));
				
				gcl->SetVertexBuffer(0, vb->GetResource());
				gcl->SetRoot32BitConstant(0, VertexOffset, 0);
				gcl->Draw(vb->GetVertexCount());

				VertexOffset += vb->GetVertexCount();
			}
	gvb_vertex_count = VertexOffset + 1;
	gcl->ResourceBarrier(gvb, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	gcl->Close();

	resource_manager->ExecuteCommandList(gcl);
	//resource_manager->FlushCommandList(cl);

	auto cl = resource_manager->GetTemporaryComputeCommandList();
	cl->Reset();

	// Build Global Node Buffer
	// -------------------------------------------------------------------------------------------------------------------------
	cl->ResourceBarrier(gnb, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	cl->SetPipelineState(TW3DShaders::GetGraphicsShader(TW3DShaders::BuildGNB));
	cl->BindUAVBuffer(0, gvb);
	TWT::UInt NodeOffset = 0;
	gvb_vertex_meshes.clear();
	for (TW3DObject* object : objects) {
		TW3DVertexMesh* mesh = object->VMInstance.VertexMesh;
		if (std::find(gvb_vertex_meshes.begin(), gvb_vertex_meshes.end(), mesh) == gvb_vertex_meshes.end()) {
		//if (gvb_vertex_meshes.find(object->VMInstance.VertexMesh) == gvb_vertex_meshes.end()) {
			//gvb_vertex_meshes.emplace(object->VMInstance.VertexMesh);
			gvb_vertex_meshes.push_back(std::pair(mesh, NodeOffset));

			mesh->SetGNBOffset(NodeOffset);
			NodeOffset += mesh->GetNodeCount();

			mesh2 = mesh;

			mesh = object->VMInstance.VertexMesh;
			//BuildVMAccelerationStructure(object->VMInstance.VertexMesh);
		}
	}
}
