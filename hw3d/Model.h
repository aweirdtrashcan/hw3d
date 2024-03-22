#pragma once

#include "DrawableBase.h"
#include "BindableBase.h"
#include "ShaderStructs.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(Graphics* gfx, std::vector<std::unique_ptr<Bindable>>&& bindables)
	{
		if (!IsStaticInitialized())
		{
			AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		}
		for (std::unique_ptr<Bindable>& bindable : bindables)
		{
			if (IndexBuffer* ibuf = dynamic_cast<IndexBuffer*>(bindable.get()))
			{
				AddIndexBuffer(std::make_unique<IndexBuffer>(*ibuf));
				bindable.release();
			}
			else
			{
				AddBind(std::move(bindable));
			}
		}

		AddBind(std::make_unique<TransformCbuf>(gfx, *this));
	}

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&model);
	}

	virtual void Update(float dt) noexcept override {}

	void Draw(Graphics* gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&model, accumulatedTransform);
		Drawable::Draw(gfx);
	}
private:
	mutable DirectX::XMFLOAT4X4 model;
};

class Node
{
	friend class Model;
public:
	Node(std::vector<Mesh*> meshes, DirectX::FXMMATRIX transform)
		:
		meshesPtrs(std::move(meshes))
	{
		DirectX::XMStoreFloat4x4(&this->transform, transform);
	}

	void Draw(Graphics* gfx, DirectX::FXMMATRIX accumulatedTransform) noexcept(!IS_DEBUG)
	{
		DirectX::XMMATRIX transform = DirectX::XMLoadFloat4x4(&this->transform) * accumulatedTransform;
		for (Mesh* mesh : meshesPtrs)
		{
			mesh->Draw(gfx, transform);
		}

		for (const std::unique_ptr<Node>& child : childPtrs)
		{
			child->Draw(gfx, transform);
		}
	}
private:
	void AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG)
	{
		childPtrs.push_back(std::move(child));
	}
private:
	std::vector<Mesh*> meshesPtrs;
	std::vector<std::unique_ptr<Node>> childPtrs;
	DirectX::XMFLOAT4X4 transform;
};

class Model
{
public:
	Model(Graphics* gfx, const std::string& fileName)
	{
		Assimp::Importer imp;
		const aiScene* scene = imp.ReadFile(fileName, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
		
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			meshPtrs.push_back(ParseMesh(gfx, scene->mMeshes[i]));
		}
		root = ParseNode(scene->mRootNode);
	}

	static std::unique_ptr<Mesh> ParseMesh(Graphics* gfx, const aiMesh* mesh)
	{
		hw3dexp::VertexBuffer vbuf(
			hw3dexp::VertexLayout()
			.Append(hw3dexp::VertexLayout::Position3D)
			.Append(hw3dexp::VertexLayout::Normal)
		);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mVertices[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i])
			);
		}

		std::vector<unsigned int> indices(mesh->mNumFaces * 3);
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			indices[i * 3] = face.mIndices[0];
			indices[i * 3 + 1] = face.mIndices[1];
			indices[i * 3 + 2] = face.mIndices[2];
		}
		
		std::vector<std::unique_ptr<Bindable>> bindables;

		bindables.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));
		bindables.push_back(std::make_unique<IndexBuffer>(gfx, indices));

		std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
		bindables.push_back(std::move(vShader));
		bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		PSMaterialConstant pmc;
		pmc.albedoColor = { 0.6f, 0.6f, 0.8f };
		pmc.specularIntensity = 0.6f;
		pmc.specularPower = 30.f;

		bindables.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1));

		bindables.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), vBlob));

		return std::make_unique<Mesh>(gfx, std::move(bindables));
	}

	std::unique_ptr<Node> ParseNode(const aiNode* node)
	{
		// Root
		DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(
			DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&node->mTransformation)
		);

		std::vector<Mesh*> currMeshes(node->mNumMeshes);
		
		for (size_t i = 0; i < currMeshes.size(); i++)
		{
			int meshIndex = node->mMeshes[i];
			currMeshes[i] = meshPtrs[meshIndex].get();
		}

		std::unique_ptr<Node> pNode = std::make_unique<Node>(std::move(currMeshes), transform);

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			aiNode* child = node->mChildren[i];
			pNode->AddChild(ParseNode(child));
		}
		
		return pNode;
	}

	void Draw(Graphics* gfx) const
	{
		root->Draw(gfx, DirectX::XMMatrixIdentity());
	}
private:
	std::unique_ptr<Node> root;
	std::vector<std::unique_ptr<Mesh>> meshPtrs;
};