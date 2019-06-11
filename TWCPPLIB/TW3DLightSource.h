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
	void SetPosition(TWT::float3 const& Position);
	void SetColor(TWT::float3 const& Color);

	// ActualColor * Intensity
	TWT::float3 GetPosition();
	TWT::float3 GetColor();

	TWT::float4 MakeInfo();
	TW3DVertexBuffer* GetTriangleVertexBuffer();

	bool Updated = false;

private:
	TW3DLightSourceType Type;

	TWT::uint triangle_id;
	TW3DVertexBuffer* triangle_vb;

	float sphere_radius;
	float intensity = 1.0f;
	TWT::float3 position;
	TWT::float3 color = TWT::float3(1);

	friend class TW3DScene;
};

