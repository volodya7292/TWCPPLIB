#include "pch.h"
#include "TW3DShaders.h"

#ifdef _DEBUG
#include "CompiledShaders/Debug/CalculateTriangleMeshBoundingBox.c.h"
#else
#include "CompiledShaders/Release/CalculateTriangleMeshBoundingBox.c.h"
#endif // _DEBUG

std::vector<TW3DComputePipelineState*> compute_shaders;
std::vector<TW3DGraphicsPipelineState*> graphics_shaders;

void TW3DShaders::Initialize(TW3DResourceManager* ResourceManager) {
	compute_shaders.resize(ComputeShaderCount);
	graphics_shaders.resize(GraphicsShaderCount);

	TW3DDevice* device = ResourceManager->GetDevice();

	TW3DRootSignature* rs0 = new TW3DRootSignature(device,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4)
		},
		false, false, false, false
	);

	auto* triangle_mesh_bounding_box_calc_ps = new TW3DComputePipelineState(rs0);
	triangle_mesh_bounding_box_calc_ps->SetShader(new TW3DShader(TW3DCompiledShader(CalculateTriangleMeshBoundingBox_ByteCode), "TriangleMeshBoundingBox"s));
	triangle_mesh_bounding_box_calc_ps->Create(device);
	compute_shaders[CalculateTriangleMeshBoundingBox] = triangle_mesh_bounding_box_calc_ps;



	// -----------------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------

}

void TW3DShaders::Release() {
	for (TW3DComputePipelineState* ps : compute_shaders)
		delete ps;
	for (TW3DGraphicsPipelineState* ps : graphics_shaders)
		delete ps;
}

TW3DComputePipelineState* TW3DShaders::GetComputeShader(TWT::uint Id) {
	return compute_shaders[Id];
}

TW3DGraphicsPipelineState* TW3DShaders::GetGraphicsShader(TWT::uint Id) {
	return graphics_shaders[Id];
}
