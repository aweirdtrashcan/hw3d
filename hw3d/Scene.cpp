#include "Scene.h"

#include "imgui.h"
#include <unordered_map>

#include "Log.h"

#ifdef _DEBUG
#include <iostream>
#endif

#include "Surface.h"

#include <format>

class SceneWindow
{
public:
    void ShowWindow(const char* windowName, const Node* rootNode)
    {
        windowName = windowName ? windowName : "Model";
        if (ImGui::Begin(windowName))
        {
            ImGui::Columns(2, nullptr, true);
            rootNode->ShowTree(selectedId, &selectedNode);

            if (!selectedNode)
                selectedNode = const_cast<Node*>(rootNode);

            ImGui::NextColumn();

            if (selectedNode != nullptr)
            {
                TransformParameters& t = selectedNode->transformParameters;
                ImGui::Text(selectedNode->name.c_str());
                ImGui::Text("Orientation");
                ImGui::SliderAngle("roll", &t.roll, -180.f, 180.f);
                ImGui::SliderAngle("pitch", &t.pitch, -180.f, 180.f);
                ImGui::SliderAngle("yaw", &t.yaw, -180.f, 180.f);
                ImGui::Text("Position");
                ImGui::SliderFloat("x", &t.x, -200.f, 200.f);
                ImGui::SliderFloat("y", &t.y, -200.f, 200.f);
                ImGui::SliderFloat("z", &t.z, -200.f, 200.f);
            }
        }
        ImGui::End();
    }

    DirectX::XMMATRIX GetTransformXM() const noexcept(!IS_DEBUG)
    {
        if (selectedNode)
        {
            const TransformParameters& param = selectedNode->transformParameters;
            return DirectX::XMMatrixRotationRollPitchYaw(param.pitch, param.yaw, param.roll) *
                DirectX::XMMatrixTranslation(param.x, param.y, param.z);
        }
        return DirectX::XMMatrixIdentity();
    }

    Node* GetSelectedNode() const { return selectedNode; }

private:
    Node* selectedNode = nullptr;
    int selectedId = 0;
};

Mesh::Mesh(Graphics* gfx, std::string meshName, std::vector<std::shared_ptr<Bindable>> bindables, DirectX::FXMMATRIX transform)
{
    AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    for (std::shared_ptr<Bindable>& bind : bindables)
    {
        AddBind(bind);
    }

    AddBind(TransformCbuf::Resolve(gfx, *this, meshName + "TransformCBuf", 2));

    DirectX::XMStoreFloat4x4(&transformation, transform);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&transformation);
}

void Mesh::Update(float dt) noexcept
{
}

void Mesh::Draw(Graphics* gfx, DirectX::FXMMATRIX transform) const noexcept(!IS_DEBUG)
{
    DirectX::XMStoreFloat4x4(&transformation, transform);
    Drawable::Draw(gfx);
}

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshes, DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG)
    :
    id(id),
    name(name),
    meshes(meshes)
{
    DirectX::XMStoreFloat4x4(&this->transform, transform);
    DirectX::XMStoreFloat4x4(&this->appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics* gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
    DirectX::XMMATRIX transform = 
        DirectX::XMLoadFloat4x4(&this->appliedTransform) *
        DirectX::XMLoadFloat4x4(&this->transform) *
        accumulatedTransform;
    
    for (Mesh* mesh : meshes)
    {
        mesh->Draw(gfx, transform);
    }

    for (const std::unique_ptr<Node>& node : childNodes)
    {
        node->Draw(gfx, transform);
    }
}

void Node::ShowTree(int& selectedId, Node** selectedNode) const noexcept(!IS_DEBUG)
{
    // if tree node expanded, recursively render all children
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    flags |= selectedId == GetId() ? ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected : 0;
    flags |= childNodes.empty() ? ImGuiTreeNodeFlags_Leaf : 0;

    bool expanded = ImGui::TreeNodeEx(name.c_str(), flags);

    if (ImGui::IsItemClicked())
    {
        selectedId = GetId();
        *selectedNode = const_cast<Node*>(this);
    }

    if (expanded)
    {
        for (const std::unique_ptr<Node>& node : childNodes)
        {
            node->ShowTree(selectedId, selectedNode);
        }
        ImGui::TreePop();
    }
}

void Node::AddChildren(std::unique_ptr<Node> children) noexcept(!IS_DEBUG)
{
    childNodes.push_back(std::move(children));
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept(!IS_DEBUG)
{
    DirectX::XMStoreFloat4x4(&this->appliedTransform, transform);
}

Scene::Scene(Graphics* gfx, std::string_view modelPath)
    :
    showWindow(std::make_unique<SceneWindow>())
{
    Assimp::Importer imp;
 
    Log::Debug(std::format("Loading model \"{}\"", modelPath));

    const aiScene* scene = imp.ReadFile(
        modelPath.data(),
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | 
        aiProcess_GenNormals | aiProcess_CalcTangentSpace
    );

    Log::Debug(std::format("The model \"{}\" was loaded!", modelPath));

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        if (scene->mNumMaterials > 0)
        {
            meshes.push_back(ParseMesh(gfx, scene->mMeshes[i], scene->mMaterials));
        }
        else if (scene->mNumMaterials == 0)
        {
            meshes.push_back(ParseMesh(gfx, scene->mMeshes[i]));
        }
    }

    rootNode = ParseNode(scene->mRootNode);
}

void Scene::Draw(Graphics* gfx)
{
    if (Node* node = showWindow->GetSelectedNode())
    {
        node->SetAppliedTransform(showWindow->GetTransformXM());
    }
    rootNode->Draw(gfx, DirectX::XMMatrixIdentity());
}

void Scene::ShowWindow(const char* windowName) noexcept
{
    showWindow->ShowWindow(windowName, rootNode.get());
}

Scene::~Scene() noexcept
{

}

std::unique_ptr<Node> Scene::ParseNode(const aiNode* node)
{
    std::vector<Mesh*> nodeMeshes(node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        nodeMeshes[i] = meshes[meshIndex].get();
    }

    DirectX::XMMATRIX nodeTransform = DirectX::XMMatrixTranspose(
        DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)&node->mTransformation)
    );
    static int id = 0;
    std::unique_ptr<Node> pNode = std::make_unique<Node>(id, node->mName.C_Str(), nodeMeshes, nodeTransform);

    id+=1;

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        pNode->AddChildren(ParseNode(node->mChildren[i]));
    }

    return pNode;
}

std::unique_ptr<Mesh> Scene::ParseMesh(Graphics* gfx, const aiMesh* mesh)
{
    return { };
}

std::unique_ptr<Mesh> Scene::ParseMesh(Graphics* gfx, const aiMesh* mesh, const aiMaterial* const* materials)
{
    hw3dexp::VertexBuffer vbuf(
        hw3dexp::VertexLayout()
        .Append(hw3dexp::VertexLayout::Position3D)
        .Append(hw3dexp::VertexLayout::Normal)
        .Append(hw3dexp::VertexLayout::Texture2D)
        .Append(hw3dexp::VertexLayout::Tangent)
        .Append(hw3dexp::VertexLayout::BiTangent)
    );

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        DirectX::XMFLOAT3* position = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mVertices[i]);
        DirectX::XMFLOAT3* normal = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);
        DirectX::XMFLOAT2 texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        DirectX::XMFLOAT3* tangent = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mTangents[i]);
        DirectX::XMFLOAT3* biTangent = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mBitangents[i]);

        vbuf.EmplaceBack(
            *position,
            *normal,
            texCoord,
            *tangent,
            *biTangent
        );
    }

    std::vector<unsigned int> indices(mesh->mNumFaces * 3);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];
        indices[i * 3 + 0] = face.mIndices[0];
        indices[i * 3 + 1] = face.mIndices[1];
        indices[i * 3 + 2] = face.mIndices[2];
    }

    std::vector<std::shared_ptr<Bindable>> bindables;

    bindables.push_back(VertexBuffer::Resolve(gfx, vbuf, std::string(mesh->mName.C_Str()) + "VertexBuffer"));
    bindables.push_back(IndexBuffer::Resolve(gfx, indices, std::string(mesh->mName.C_Str()) + "IndexBuffer"));

    bool hasSpecular = false;
    bool hasNormals = false;

    PSMaterialConstant psMaterial{};
    psMaterial.diffuseColor = { 1.0f, 1.0f, 1.0f };
    psMaterial.specularIntensity = 1.0f;

    if (mesh->mMaterialIndex >= 0)
    {
        const aiMaterial* material = materials[mesh->mMaterialIndex];

        material->Get(AI_MATKEY_COLOR_DIFFUSE, psMaterial.diffuseColor);

        aiString texFileName;
        
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
        {
            std::string texPath = std::format("Models/{}", texFileName.C_Str());

            Log::Debug(std::format("Loading diffuse texture \"{}\"", texPath));
            bindables.push_back(Texture::Resolve(gfx, texPath));
            Log::Debug(std::format("Diffuse texture \"{}\" was loaded", texPath));
            
            bindables.push_back(Sampler::Resolve(gfx));
        }

        if (material->GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
        {
            std::string texPath = std::format("Models/{}", texFileName.C_Str());
            hasSpecular = true;

            Log::Debug(std::format("Loading specular texture \"{}\"", texPath));
            bindables.push_back(Texture::Resolve(gfx, texPath, 1));
            Log::Debug(std::format("Specular texture \"{}\" was loaded", texPath));

            bindables.push_back(Sampler::Resolve(gfx));
        }
        else
        {
            float shininess;
            material->Get(AI_MATKEY_SHININESS, shininess);
            psMaterial.specularPower = shininess;
        }

        if (material->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
        {
            std::string texPath = std::format("Models/{}", texFileName.C_Str());
            hasNormals = true;

            Log::Debug(std::format("Loading normals texture \"{}\"", texPath));
            bindables.push_back(Texture::Resolve(gfx, texPath, 2));
            Log::Debug(std::format("Normals texture \"{}\" was loaded", texPath));

            bindables.push_back(Sampler::Resolve(gfx));
        }
    }

    if (hasNormals)
    {
        if (hasSpecular)
        {
            std::shared_ptr<Bindable> vShader = VertexShader::Resolve(gfx, "PhongVSTex.cso");
            Microsoft::WRL::ComPtr<ID3DBlob> vBlob = std::static_pointer_cast<VertexShader>(vShader)->GetBytecode();
            bindables.push_back(vShader);

            bindables.push_back(PixelShader::Resolve(gfx, "PhongPSTexNorm.cso"));

            bindables.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), vBlob));
        }
        else
        {
            std::shared_ptr<Bindable> vShader = VertexShader::Resolve(gfx, "PhongVSTex.cso");
            Microsoft::WRL::ComPtr<ID3DBlob> vBlob = std::static_pointer_cast<VertexShader>(vShader)->GetBytecode();
            bindables.push_back(vShader);

            bindables.push_back(PixelShader::Resolve(gfx, "PhongPSTexNoSpec.cso"));

            bindables.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), vBlob));
        }
    }
    else
    {
        std::shared_ptr<Bindable> vShader = VertexShader::Resolve(gfx, "PhongVSTex.cso");
        Microsoft::WRL::ComPtr<ID3DBlob> vBlob = std::static_pointer_cast<VertexShader>(vShader)->GetBytecode();
        bindables.push_back(vShader);

        bindables.push_back(PixelShader::Resolve(gfx, "PhongPSTex.cso"));

        bindables.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), vBlob));
    }

    bindables.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, psMaterial, 1));

    return std::make_unique<Mesh>(gfx, mesh->mName.C_Str(), std::move(bindables));
}
