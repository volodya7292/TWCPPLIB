#pragma once
#include "TW3DResourceManager.h"
#include "TW3DModules.h"

struct LBVHMortonCode {
	TWT::vec4u data; // .x - code, .y - index
};

struct LBVHNode {
	TWT::Bounds bounds;
	TWT::uint primitive_index;
	TWT::uint parent;
	TWT::uint left_child;
	TWT::uint right_child;
};

struct SceneLBVHInstance {
	TWT::uint GVBOffset;
	TWT::uint GNBOffset;
	TWT::mat4 Transform;
	TWT::mat4 TransformInverse;
};

struct SceneLBVHNode {
	SceneLBVHInstance instance;
	TWT::Bounds bounds;
	TWT::uint parent;
	TWT::uint left_child;
	TWT::uint right_child;
};

class TW3DLBVH {
public:
	TW3DLBVH(TW3DResourceManager* ResourceManager, TWT::uint ElementCount, bool SceneLevel = false);
	~TW3DLBVH();

	TWT::uint GetNodeCount();
	TW3DBuffer* GetNodeBuffer();

	void BuildFromTriangles(TW3DBuffer* GVB, TWT::uint GVBOffset, TW3DGraphicsCommandList* CommandList = nullptr);
	void BuildFromLBVHs(TW3DBuffer* GNB, TW3DBuffer* SceneMeshInstancesBuffer, TW3DGraphicsCommandList* CommandList);

private:
	TW3DResourceManager* resource_manager;

	TW3DBuffer* bounding_box_buffer;
	TW3DBuffer* morton_codes_buffer;
	TW3DBuffer* node_buffer;

	TWT::uint element_count;
};