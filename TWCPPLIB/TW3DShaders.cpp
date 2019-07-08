#include "pch.h"
#include "TW3DShaders.h"

#ifdef _DEBUG
#include "CompiledShaders/Debug/CalculateTriangleMeshBoundingBox.c.h"
#else
#include "CompiledShaders/Release/CalculateTriangleMeshBoundingBox.c.h"
#endif // _DEBUG

std::vector<TW3DComputePipeline*> compute_shaders;
std::vector<TW3DGraphicsPipeline*> graphics_shaders;

void TW3DShaders::Initialize(TW3DResourceManager* ResourceManager) {
	compute_shaders.resize(ComputeShaderCount);
	graphics_shaders.resize(GraphicsShaderCount);

	TW3DRootSignature* rs0 = ResourceManager->RequestRootSignature("tri_mesh_bb_calc_rs"s,
		{
			TW3DRPBuffer(0, D3D12_SHADER_VISIBILITY_ALL, 0),
			TW3DRPBuffer(1, D3D12_SHADER_VISIBILITY_ALL, 0, true),
			TW3DRPConstants(2, D3D12_SHADER_VISIBILITY_ALL, 0, 4)
		},
		false, false, false, false
	);

	auto triangle_mesh_bounding_box_calc_s = ResourceManager->RequestShader("CalculateTriangleMeshBoundingBoxShader"s, TW3DCompiledShader(CalculateTriangleMeshBoundingBox_ByteCode));

	auto triangle_mesh_bounding_box_calc_ps = ResourceManager->RequestComputePipeline("tri_mesh_bb_calc_cp"s, rs0, triangle_mesh_bounding_box_calc_s);
	compute_shaders[CalculateTriangleMeshBoundingBox] = triangle_mesh_bounding_box_calc_ps;



	// -----------------------------------------------------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------

}

TW3DComputePipeline* TW3DShaders::GetComputeShader(TWT::uint Id) {
	return compute_shaders[Id];
}

TW3DGraphicsPipeline* TW3DShaders::GetGraphicsShader(TWT::uint Id) {
	return graphics_shaders[Id];
}
