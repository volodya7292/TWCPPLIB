#pragma once
#include "TW3DResourceManager.h"
#include "TW3DModules.h"

struct LBVHMortonCode {
	TWT::Vector4u data; // .x - code, .y - index
};

struct LBVHNode {
	TWT::Bounds bounds;
	TWT::UInt primitive_index;
	TWT::UInt parent;
	TWT::UInt left_child;
	TWT::UInt right_child;
};

struct SceneLBVHInstance {
	TWT::UInt GVBOffset;
	TWT::UInt GNBOffset;
	TWT::Matrix4f Transform;
	TWT::Matrix4f TransformInverse;
};

struct SceneLBVHNode {
	SceneLBVHInstance instance;
	TWT::Bounds bounds;
	TWT::UInt parent;
	TWT::UInt left_child;
	TWT::UInt right_child;
};

class TW3DLBVH {
public:
	TW3DLBVH(TW3DResourceManager* ResourceManager, TWT::UInt ElementCount, TWT::Bool SceneLevel = false);
	~TW3DLBVH();

	TWT::UInt GetNodeCount();
	TW3DResourceUAV* GetNodeBuffer();

	void BuildFromTriangles(TW3DResourceUAV* GVB, TWT::UInt GVBOffset, TW3DGraphicsCommandList* CommandList = nullptr);
	void BuildFromLBVHs(TW3DResourceUAV* GNB, TW3DResourceUAV* SceneMeshInstancesBuffer, TW3DGraphicsCommandList* CommandList);

private:
	TW3DResourceManager* resource_manager;

	TW3DResourceUAV* bounding_box_buffer;
	TW3DResourceUAV* morton_codes_buffer;
	TW3DResourceUAV* node_buffer;

	TWT::UInt element_count;
};