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

		std::vector<Vertex> vertices(mesh->mNumVertices);
		
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.pos = *(DirectX::XMFLOAT3*)&mesh->mVertices[i];
			vertex.n = *(DirectX::XMFLOAT3*)&mesh->mNormals[i];

			vertices[i] = vertex;
		}

		std::vector<unsigned short> indices(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			indices[i * 3 + 0] = face.mIndices[0];
			indices[i * 3 + 1] = face.mIndices[1];
			indices[i * 3 + 2] = face.mIndices[2];
		}

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
		AddStaticBind(std::move(vShader));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vBlob));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	materialConstants.albedoColor = { 1.0f, 1.0f, 1.0f };

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, materialConstants, 1));
	
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixIdentity()
	);
}
