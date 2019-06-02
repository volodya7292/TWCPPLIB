#include "pch.h"
#include "TW3DPrimitives.h"

static TW3DVertexMesh* Cube;
static TW3DVertexMesh* Pyramid4;

void TW3DPrimitives::Initialize(TW3DResourceManager* ResourceManager) {
	TW3DVertexBuffer* CubeVertexBuffer = ResourceManager->CreateVertexBuffer(36, sizeof(TWT::DefaultVertex));
	
	TWT::DefaultVertex vertices[] = {
		// front face
		{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },

		// right side face
		{ 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f },

		// left side face
		{ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
		{ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
		{ -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f },

		// back face
		{ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
		{ 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
		{ -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
		{ 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
		{ -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
		{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f },

		// top face
		{ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },

		// bottom face
		{ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f },
		{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f },
		{ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f },
		{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f },
	};

	//TWU::TW3DCalculateTriangleNormals(vertices, 36, fsizeof(TWT::DefaultVertex), foffsetof(TWT::DefaultVertex, Pos), foffsetof(TWT::DefaultVertex, TexCoord),
	//	foffsetof(TWT::DefaultVertex, Normal), foffsetof(TWT::DefaultVertex, Tangent), foffsetof(TWT::DefaultVertex, Bitangent));

	CubeVertexBuffer->Update(vertices, sizeof(vertices) / sizeof(TWT::DefaultVertex));
	Cube = new TW3DVertexMesh(ResourceManager, {CubeVertexBuffer});


	TW3DVertexBuffer* Pyramid4VertexBuffer = ResourceManager->CreateVertexBuffer(18, sizeof(TWT::DefaultVertex));

	//TWT::DefaultVertex vertices2[] = {
	//	// front face
	//	{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	//{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	//{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	//{ -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	//{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	//{ 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f },

	//// right side face
	//{ 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	//{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f },
	//{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
	//{ 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	//{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
	//{ 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f },

	//// left side face
	//{ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
	//{ -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f },
	//{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
	//{ -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
	//{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
	//{ -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f },
	//};

	TWT::DefaultVertex vertices2[] = {
	// bottom face
	{ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f },
	{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f },
	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f },
	{ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f },
	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f },
	{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f },

	// side faces
	{ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	{ 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },

	{ -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },

	{ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },

	{ 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	{ -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	};

	TWU::TW3DCalculateTriangleNormals(vertices2, 18, fsizeof(TWT::DefaultVertex), foffsetof(TWT::DefaultVertex, Pos), foffsetof(TWT::DefaultVertex, TexCoord),
		foffsetof(TWT::DefaultVertex, Normal), foffsetof(TWT::DefaultVertex, Tangent), foffsetof(TWT::DefaultVertex, Bitangent));

	Pyramid4VertexBuffer->Update(vertices2, sizeof(vertices2) / sizeof(TWT::DefaultVertex));
	Pyramid4 = new TW3DVertexMesh(ResourceManager, { Pyramid4VertexBuffer });
}

void TW3DPrimitives::Release() {
	delete Cube->VertexBuffers[0];
	delete Cube;
	delete Pyramid4->VertexBuffers[0];
	delete Pyramid4;
}

TW3DVertexMesh* TW3DPrimitives::GetCubeVertexMesh() {
	return Cube;
}

TW3DVertexMesh* TW3DPrimitives::GetPyramid4VertexMesh() {
	return Pyramid4;
}
