#pragma once
#include "TW3DResourceManager.h"
#include "TW3DModules.h"

namespace TW3D {
	class TW3DLBVH {
	public:
		TW3DLBVH(TW3DResourceManager* ResourceManager, TWT::UInt ElementCount);
		~TW3DLBVH();

		TWT::UInt GetNodeCount();
		TW3DResourceUAV* GetNodeBuffer();

		void BuildFromPrimitives(TW3DResourceUAV* GVB, TWT::UInt GVBOffset);
		void BuildFromLBVHs(TW3DResourceUAV* GNB, TWT::UInt GNBOffset);

	private:
		TW3DResourceManager* resource_manager;

		TW3DResourceUAV* bounding_box_buffer;
		TW3DResourceUAV* morton_codes_buffer;
		TW3DResourceUAV* morton_indices_buffer;
		TW3DResourceUAV* node_buffer = 0;

		TWT::Bool resources_initialized = false;
		TWT::UInt element_count;
		TWT::UInt node_count;
	};
}