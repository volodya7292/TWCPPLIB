#include "pch.h"
#include "TW3DLightSource.h"

TW3DLightSource::TW3DLightSource(TW3DVertexBuffer* VertexBuffer, TWT::uint TriangleId, TWT::uint MaterialId, TWT::uint VertexByteSize) :
	type(TW3D_LIGHT_SOURCE_TRIANGLE) {

	SetTriangle(VertexBuffer, TriangleId, MaterialId, VertexByteSize);
}

TW3DLightSource::TW3DLightSource(TWT::float3 SpherePosition, float SphereRadius) :
	type(TW3D_LIGHT_SOURCE_SPHERE) {
	
	SetSpherePosition(SpherePosition);
	SetSphereRadius(SphereRadius);
}

void TW3DLightSource::SetTriangle(TW3DVertexBuffer* VertexBuffer, TWT::uint TriangleId, TWT::uint MaterialId, TWT::uint VertexByteSize) {
	triangle_material_id = MaterialId;

	TWT::DefaultVertex v[3];
	VertexBuffer->Read(&v, VertexByteSize * 3 * TriangleId, VertexByteSize * 3);

	triangle[0] = v[0].Pos;
	triangle[1] = v[1].Pos;
	triangle[2] = v[2].Pos;
	triangle_normal = v[0].Normal;
}

void TW3DLightSource::SetSpherePosition(TWT::float3 Position) {
	sphere_pos = Position;
	Updated = true;
}

void TW3DLightSource::SetSphereRadius(float Radius) {	
	sphere_radius = Radius;
	Updated = true;
}

void TW3DLightSource::SetIntensity(float Intensity) {
	intensity = Intensity;
	Updated = true;
}

void TW3DLightSource::SetColor(TWT::float3 Color) {
	color = Color;
	Updated = true;
}

void TW3DLightSource::SetType(TW3DLightSourceType Type) {
	type = Type;
	Updated = true;
}

const TWT::DefaultLightSource TW3DLightSource::MakeInfo() const{
	TWT::DefaultLightSource ls;

	ls.info.x = type;

	if (type == TW3D_LIGHT_SOURCE_TRIANGLE) {
		ls.v[0] = TWT::float4(triangle[0], 1);
		ls.v[1] = TWT::float4(triangle[1], 1);
		ls.v[2] = TWT::float4(triangle[2], 1);
		ls.normal = TWT::float4(triangle_normal, triangle_material_id);
	} else if (type == TW3D_LIGHT_SOURCE_SPHERE) {
		ls.v[0] = TWT::float4(sphere_pos, 1);
		ls.info.y = sphere_radius;
	}

	return ls;
}