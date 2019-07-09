#pragma once
#include "TW3DVertexBuffer.h"
#include "TW3DTypes.h"

enum TW3DLightSourceType {
	TW3D_LIGHT_SOURCE_TRIANGLE,
	TW3D_LIGHT_SOURCE_SPHERE
};

class TW3DLightSource {
public:
	TW3DLightSource() = default;
	TW3DLightSource(TW3DVertexBuffer* VertexBuffer, TWT::uint TriangleId, TWT::uint MaterialId, TWT::uint VertexByteSize = sizeof(TWT::DefaultVertex));
	TW3DLightSource(TWT::float3 SpherePosition, float SphereRadius);

	void SetTriangle(TW3DVertexBuffer* VertexBuffer, TWT::uint TriangleId, TWT::uint MaterialId, TWT::uint VertexByteSize = sizeof(TWT::DefaultVertex));
	void SetSpherePosition(TWT::float3 Position);
	void SetSphereRadius(float Radius);
	void SetIntensity(float Intensity);
	void SetColor(TWT::float3 Color);
	void SetType(TW3DLightSourceType Type);

	const TWT::DefaultLightSource MakeInfo() const;

	bool Updated = true;

private:
	TW3DLightSourceType type = TW3D_LIGHT_SOURCE_SPHERE;

	TWT::float3 triangle[3];
	TWT::float3 triangle_normal;
	TWT::float3 sphere_pos = TWT::float3(0);

	TWT::uint triangle_material_id;
	float sphere_radius = 0.0f;
	float intensity = 1.0f;
	TWT::float3 color = TWT::float3(1);
};