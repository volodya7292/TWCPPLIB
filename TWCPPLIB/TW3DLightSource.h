#pragma once
#include "TW3DVertexBuffer.h"

enum TW3DLightSourceType {
	TW3D_LIGHT_SOURCE_TRIANGLE,
	TW3D_LIGHT_SOURCE_SPHERE
};

class TW3DLightSource {
public:
	TW3DLightSource() = default;
	void SetTriangleId(TWT::uint TriangleId, TW3DVertexBuffer* VertexBuffer);
	void SetSphereRadius(float Radius);
	void SetIntensity(float Intensity);
	void SetPosition(TWT::vec3 const& Position);
	void SetColor(TWT::vec3 const& Color);

	// ActualColor * Intensity
	TWT::vec3 GetPosition();
	TWT::vec3 GetColor();

	TWT::vec4 MakeInfo();
	TW3DVertexBuffer* GetTriangleVertexBuffer();

	bool Updated = false;

private:
	TW3DLightSourceType Type;

	TWT::uint triangle_id;
	TW3DVertexBuffer* triangle_vb;

	float sphere_radius;
	float intensity = 1.0f;
	TWT::vec3 position;
	TWT::vec3 color = TWT::vec3(1);

	friend class TW3DScene;
};

