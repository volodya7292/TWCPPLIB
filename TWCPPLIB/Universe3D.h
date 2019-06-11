#pragma once
#include "TW3DObject.h"
#include "UniverseObject.h"

/*
	UNIVERSE SIZE = 2^48 * 10 kM = 2,814,749 billion kM
	AVG. DISTANCE BETWEEN GALAXIES = 2,814 billion kM
*/

class Universe3D : public TW3DObject {
public:
	Universe3D(TW3DResourceManager* ResourceManager);
	~Universe3D() override;

private:
	// Position: Universe relative position
	// Extent: Area extent(radius)
	std::vector<UniverseObject> GatherObjects(TWT::double3 Position, TWT::float3 Extent);
};