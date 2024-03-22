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
	Mesh(Graphics* gfx, std::vector<std::unique_ptr<Bindable>> bindables, DirectX::FXMMATRIX transform = DirectX::XMMatrixIdentity());

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
	virtual void Update(float dt) noexcept override;
	void Draw(Graphics* gfx, DirectX::FXMMATRIX transform) const noexcept(!IS_DEBUG);
private:
	mutable DirectX::XMFLOAT4X4 transformation;
};

class Node
{
	friend class Scene;
	friend class SceneWindow;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshes, DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG);
	void Draw(Graphics* gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void ShowTree(int& selectedId, Node** selectedNode) const noexcept(!IS_DEBUG);
	int GetId() const noexcept { return id; }
private:
	void AddChildren(std::unique_ptr<Node> children) noexcept(!IS_DEBUG);
private:
	std::string name;
	std::vector<Mesh*> meshes;
	std::vector<std::unique_ptr<Node>> childNodes;
	DirectX::XMFLOAT4X4 transform;
	int id;
};

class Scene
{
	friend class SceneWindow;
public:
	Scene(Graphics* gfx, const std::string& modelPath);
	void Draw(Graphics* gfx);
	void ShowWindow(const char* windowName = nullptr) noexcept;
	~Scene() noexcept;
private:
	std::unique_ptr<Node> ParseNode(const aiNode* node);
	std::unique_ptr<Mesh> ParseMesh(Graphics* gfx, const aiMesh* mesh);
private:
	std::vector<std::unique_ptr<Mesh>> meshes;
	std::unique_ptr<Node> rootNode;
	std::unique_ptr<class SceneWindow> showWindow;
};