#pragma once
#include "TW3DResourceManager.h"
#include "TW3DVertexMesh.h"

namespace TW3D {
	class TW3DRTAccelerationStructureTL
	{
	public:
		TW3DRTAccelerationStructureTL(const TWT::Vector<TW3DVertexMeshInstance*>& VertexMeshInstances);
		~TW3DRTAccelerationStructureTL();

		void AddBLASInstance(TW3DVertexMeshInstance* BLASInstance);

		void Build();

	private:
		// Global Vertex Buffer
		TW3DResourceVB* gvb;

		TWT::Vector<TW3DVertexMeshInstance*> BLAS_instances;
	};
}