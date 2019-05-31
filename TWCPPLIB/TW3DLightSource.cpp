#include "pch.h"
#include "TW3DLightSource.h"

void TW3DLightSource::SetTriangleId(TWT::uint TriangleId) {
	Type = TW3D_LIGHT_SOURCE_TRIANGLE;
	triangle_id = TriangleId;
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

void TW3DLightSource::SetPosition(TWT::vec3 const& Position) {
	position = Position;
	Updated = true;
}

void TW3DLightSource::SetColor(TWT::vec3 const& Color) {
	color = Color;
	Updated = true;
}

TWT::vec3 TW3DLightSource::GetPosition() {
	return TWT::vec3();
}

TWT::vec3 TW3DLightSource::GetColor() {
	return color * intensity;
}

TWT::vec4 TW3DLightSource::MakeInfo() {
	return TWT::vec4(Type, triangle_id, sphere_radius, 0); // .x - type, .y - triangle id, z - sphere radius
}
