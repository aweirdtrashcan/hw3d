#include "Suzanne.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "BindableBase.h"

Suzanne::Suzanne(Graphics* gfx,
	std::mt19937& rng, 
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	std::uniform_real_distribution<float>& bdist)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist, bdist)
{
	
	if (!IsStaticInitialized())
	{
		Assimp::Importer imp;
		const aiScene* scene = imp.ReadFile("Models\\suzanne.obj", 
			aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
		);

		const aiMesh* mesh = scene->mMeshes[0];

		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.pos = *(DirectX::XMFLOAT3*)&mesh->mVertices[i];
			vertex.n = *(DirectX::XMFLOAT3*)&mesh->mNormals[i];

			vertices.push_back(vertex);
		}

		std::vector<unsigned int> indices;
		indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
		AddStaticBind(std::make_unique<IndexBuffer>(gfx, indices));

		std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, "PhongVS.cso");
		Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
		AddStaticBind(std::move(vShader));

		AddStaticBind(std::make_unique<PixelShader>(gfx, "PhongPS.cso"));
	}
}
