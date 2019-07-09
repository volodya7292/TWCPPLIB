#include "pch.h"
#include "TW3DPrimitives.h"

static TW3DVertexBuffer* Cube;
static TW3DVertexBuffer* Pyramid4;

void TW3DPrimitives::Initialize(TW3DResourceManager* ResourceManager) {
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

	vertices[23].TexCoord.z = 1;
	vertices[24].TexCoord.z = 1;
	vertices[25].TexCoord.z = 1;
	vertices[26].TexCoord.z = 1;
	vertices[27].TexCoord.z = 1;
	vertices[28].TexCoord.z = 1;





	//Assimp::Importer importer;
	//const aiScene* sc = importer.ReadFile("D:/room.obj", aiProcess_FlipUVs);


	/*TW3DVertexBuffer* CubeVertexBuffer = ResourceManager->RequestVertexBuffer("cube_vb"s, vertices.size(), sizeof(TWT::DefaultVertex));
	CubeVertexBuffer->Update(vertices.data(), vertices.size());*/


	//CubeVertexBuffer->Update(vertices, sizeof(vertices) / sizeof(TWT::DefaultVertex));
	
	

	//std::vector<TWT::DefaultVertex> vertices3;

	//// Iterate through all meshes in the file and extract the vertex information used in this demo
	//for (uint32_t m = 0; m < sc->mNumMeshes; m++) {

	//	TWU::CPrintln(sc->mMeshes[m]->mNumVertices);
	//	for (uint32_t v = 0; v < sc->mMeshes[m]->mNumVertices; v++) {
	//		TWT::DefaultVertex vertex;


	//		vertex.Pos = glm::make_vec3(&sc->mMeshes[m]->mVertices[v].x);
	//		vertex.TexCoord = TWT::float3(glm::make_vec2(&sc->mMeshes[m]->mTextureCoords[0][v].x), 0);
	//		vertex.Normal = glm::make_vec3(&sc->mMeshes[m]->mNormals[v].x);

	//		//vertex.normal = glm::make_vec3(&sc->mMeshes[m]->mNormals[v].x);

	//		//vertex.color = (sc->mMeshes[m]->HasVertexColors(0)) ? glm::make_vec3(&sc->mMeshes[m]->mColors[0][v].r) : glm::vec3(1.0f);

	//		// Fetch bone weights and IDs
	//		/*for (uint32_t j = 0; j < MAX_BONES_PER_VERTEX; j++) {
	//			vertex.boneWeights[j] = skinnedMesh->bones[vertexBase + v].weights[j];
	//			vertex.boneIDs[j] = skinnedMesh->bones[vertexBase + v].IDs[j];
	//		}*/

	//		vertices3.push_back(vertex);
	//	}

	//	
	//	
	//	
	//	//vertexBase += skinnedMesh->scene->mMeshes[m]->mNumVertices;
	//}

	//TW3DVertexBuffer* vb = ResourceManager->RequestVertexBuffer("cube_vb"s, vertices3.size(), sizeof(TWT::DefaultVertex));
	//vb->Update(vertices3.data(), vertices3.size());
	//vbs.push_back(vb);


	Cube = ResourceManager->RequestVertexBuffer("cube_vb"s, sizeof(vertices) / sizeof(TWT::DefaultVertex), sizeof(TWT::DefaultVertex));
	Cube->Update(vertices, sizeof(vertices) / sizeof(TWT::DefaultVertex));











	TW3DVertexBuffer* Pyramid4VertexBuffer = ResourceManager->RequestVertexBuffer("pyramid_vb"s, 18, sizeof(TWT::DefaultVertex));

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
		foffsetof(TWT::DefaultVertex, Normal), foffsetof(TWT::DefaultVertex, Tangent));

	Pyramid4VertexBuffer->Update(vertices2, sizeof(vertices2) / sizeof(TWT::DefaultVertex));

	//Pyramid4VertexBuffer->Update(vertices2, sizeof(vertices2) / sizeof(TWT::DefaultVertex));
	//Pyramid4 = new TW3DVertexMesh(ResourceManager, { Pyramid4VertexBuffer });
}

TW3DVertexBuffer* TW3DPrimitives::GetCubeVertexMesh() {
	return Cube;
}

TW3DVertexBuffer* TW3DPrimitives::GetPyramid4VertexMesh() {
	return Pyramid4;
}
