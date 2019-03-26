#pragma once
#include "TW3DResourceVB.h"
#include "TW3DResourceCB.h"
#include "TW3DTypes.h"

namespace TW3D {
	class TW3DResourceManager {
	public:
		TW3DResourceManager(TW3DDevice* Device);
		~TW3DResourceManager();

		// By default SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex)
		TW3DResourceVB* CreateVertexBuffer(TWT::UInt VertexCount, TWT::UInt SingleVertexSizeInBytes = sizeof(TWT::DefaultVertex));
		TW3DResourceCB* CreateConstantBuffer(TWT::UInt ElementSizeInBytes, TWT::UInt ElementCount = 1);

	private:
		TW3DDevice* device;
		TW3DTempGCL* temp_gcl;
	};
}