#pragma once
#include "TW3DResourceManager.h"
#include "TW3DModules.h"

namespace TW3D {
	struct LBVHNode {
		TWT::Bounds bounds;
		TWT::UInt primitive_index;
		TWT::UInt parent;
		TWT::UInt left_child;
		TWT::UInt right_child;
	};

	class TW3DLBVH {
	public:
		TW3DLBVH(TW3DResourceManager* ResourceManager, TWT::UInt ElementCount);
		~TW3DLBVH();

		TWT::UInt GetNodeCount();
		TW3DResourceUAV* GetNodeBuffer();

		void BuildFromPrimitives(TW3DResourceUAV* GVB, TWT::UInt GVBOffset);
		void BuildFromLBVHs(TW3DResourceUAV* GNB, const TWT::Vector<TWT::UInt>& GNBOffsets);

	private:
		TW3DResourceManager* resource_manager;

		TW3DResourceUAV* bounding_box_buffer;
		TW3DResourceUAV* morton_codes_buffer;
		TW3DResourceUAV* morton_indices_buffer;
		TW3DResourceUAV* node_buffer;

		TWT::UInt element_count;
		TWT::UInt node_count;
	};
}