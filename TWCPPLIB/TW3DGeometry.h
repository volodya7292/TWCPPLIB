#pragma once
#include "TW3DResourceVB.h"
#include "TW3DObject.h"

namespace TW3D {
	class TW3DGeometry : public TW3DObject {
	public:
		TW3DGeometry() = default;
		virtual ~TW3DGeometry() = default;

	private:
		TW3DResourceVB* VertexBuffer;
	};
}