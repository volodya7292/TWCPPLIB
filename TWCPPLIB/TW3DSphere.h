#pragma once
#include "TW3DGeometry.h"

class TW3DSphere : public TW3DGeometry {
public:
	TW3DSphere(TW3DResourceManager* ResourceManager);
	~TW3DSphere() = default;
};