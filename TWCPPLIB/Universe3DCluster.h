#pragma once
#include "TW3DTypes.h"

/*
	UNIVERSE SIZE = 2^48 * 10 kM = 2,814,749 billion kM
*/

class Universe3DCluster {
public:
	Universe3DCluster();
	~Universe3DCluster();

	TWT::vec3d universe_pos;

	const TWT::vec3u SIZE = TWT::vec3u(3, 3, 3);
};