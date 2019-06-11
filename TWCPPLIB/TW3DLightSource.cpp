#include "pch.h"
#include "TW3DLightSource.h"

void TW3DLightSource::SetTriangleId(TWT::uint TriangleId, TW3DVertexBuffer* VertexBuffer) {
	Type = TW3D_LIGHT_SOURCE_TRIANGLE;
	triangle_id = TriangleId;
	triangle_vb = VertexBuffer;
	Updated = true;
}

void TW3DLightSource::SetSphereRadius(float Radius) {
	Type = TW3D_LIGHT_SOURCE_SPHERE;
	sphere_radius = Radius;
	Updated = true;
}

void TW3DLightSource::SetIntensity(float Intensity) {
	intensity = Intensity;
	Updated = true;
}

void TW3DLightSource::SetPosition(TWT::float3 const& Position) {
	position = Position;
	Updated = true;
}

void TW3DLightSource::SetColor(TWT::float3 const& Color) {
	color = Color;
	Updated = true;
}

TWT::float3 TW3DLightSource::GetPosition() {
	return position;
}

TWT::float3 TW3DLightSource::GetColor() {
	return color * intensity;
}

TWT::float4 TW3DLightSource::MakeInfo() {
	return TWT::float4(Type, triangle_id, sphere_radius, 0); // .x - type, .y - GVB triangle id, z - sphere radius
}

TW3DVertexBuffer* TW3DLightSource::GetTriangleVertexBuffer() {
	return triangle_vb;
}
