#include "Scene.h"

#include "imgui.h"
#include <unordered_map>

#include "Log.h"

#ifdef _DEBUG
#include <iostream>
#endif

#include "Surface.h"

#include <format>

std::unordered_map<std::string, std::shared_ptr<Bindable>> g_SharedBindables;

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

Mesh::Mesh(Graphics* gfx, std::vector<std::unique_ptr<Bindable>> bindables, std::vector<std::shared_ptr<Bindable>> sharedBindables, DirectX::FXMMATRIX transform)
{
    if (!IsStaticInitialized())
    {
        AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }

    for (std::unique_ptr<Bindable>& bind : bindables)
    {
        if (IndexBuffer* ibuf = dynamic_cast<IndexBuffer*>(bind.get()))
        {
            AddIndexBuffer(std::make_unique<IndexBuffer>(*ibuf));
            bind.release();
            continue;
        }
        AddBind(std::move(bind));
    }
    
    for (const std::shared_ptr<Bindable>& bind : sharedBindables)
    {
        AddSharedBind(bind);
    }

    AddBind(std::make_unique<TransformCbuf>(gfx, *this, 2));

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
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded | aiProcess_GenNormals
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
    hw3dexp::VertexBuffer vbuf(
        hw3dexp::VertexLayout()
            .Append(hw3dexp::VertexLayout::Position3D)
            .Append(hw3dexp::VertexLayout::Normal)
    );

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        DirectX::XMFLOAT3* position = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mVertices[i]);
        DirectX::XMFLOAT3* normal = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);

        vbuf.EmplaceBack(
            *position, 
            *normal
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

    std::vector<std::unique_ptr<Bindable>> bindables;

    bindables.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));
    bindables.push_back(std::make_unique<IndexBuffer>(gfx, indices));
    
    std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
    Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
    bindables.push_back(std::move(vShader));

    bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

    bindables.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), vBlob));

    PSMaterialConstant material;
    material.albedoColor = { 0.6f, 0.6f, 0.8f };
    material.fresnelR0 = { 0.7f, 0.7f, 0.7f };

    bindables.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, material, 1));

    return std::make_unique<Mesh>(gfx, std::move(bindables));
}

std::unique_ptr<Mesh> Scene::ParseMesh(Graphics* gfx, const aiMesh* mesh, const aiMaterial* const* materials)
{
    hw3dexp::VertexBuffer vbuf(
        hw3dexp::VertexLayout()
        .Append(hw3dexp::VertexLayout::Position3D)
        .Append(hw3dexp::VertexLayout::Normal)
        .Append(hw3dexp::VertexLayout::Texture2D)
    );

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        DirectX::XMFLOAT3* position = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mVertices[i]);
        DirectX::XMFLOAT3* normal = reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[i]);
        DirectX::XMFLOAT2 texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

        vbuf.EmplaceBack(
            *position,
            *normal,
            texCoord
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

    std::vector<std::unique_ptr<Bindable>> bindables;
    std::vector<std::shared_ptr<Bindable>> sharedBindables;

    bindables.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));
    bindables.push_back(std::make_unique<IndexBuffer>(gfx, indices));

    bool hasSpecular = false;
    bool hasNormals = false;

    PSMaterialConstant psMaterial;

    if (mesh->mMaterialIndex >= 0)
    {
        const aiMaterial* material = materials[mesh->mMaterialIndex];

        for (unsigned int i = 0; i < material->mNumProperties; i++)
        {
            aiMaterialProperty* property = material->mProperties[i];
            
            if (strcmp(property->mKey.C_Str(), "$clr.diffuse") == 0)
            {
                memcpy(&psMaterial.albedoColor, property->mData, property->mDataLength);
                break;
            }
        }

        aiString texFileName;
        
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
        {
            std::string texPath = std::format("Models/{}", texFileName.C_Str());

            if (g_SharedBindables[texPath] == nullptr)
            {
                Log::Debug(std::format("Loading diffuse texture \"{}\"", texPath));
                g_SharedBindables[texPath] = std::make_shared<Texture>(gfx, Surface::FromFile(texPath));
                Log::Debug(std::format("Diffuse texture \"{}\" was loaded", texPath));
            }
            else
            {
                Log::Debug(std::format("diffuse texture \"{}\" was already loaded!", texPath));
            }

            sharedBindables.push_back(g_SharedBindables[texPath]);
            
            bindables.push_back(std::make_unique<Sampler>(gfx));
        }

        if (material->GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
        {
            std::string texPath = std::format("Models/{}", texFileName.C_Str());
            hasSpecular = true;

            if (g_SharedBindables[texPath] == nullptr)
            {
                Log::Debug(std::format("Loading specular texture \"{}\"", texPath));
                g_SharedBindables[texPath] = std::make_shared<Texture>(gfx, Surface::FromFile(texPath), 1);
                Log::Debug(std::format("Specu lar texture \"{}\" was loaded", texPath));
            }
            else
            {
                Log::Debug(std::format("specular texture \"{}\" was already loaded!", texPath));
            }

            sharedBindables.push_back(g_SharedBindables[texPath]);
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

            if (g_SharedBindables[texPath] == nullptr)
            {
                Log::Debug(std::format("Loading normals texture \"{}\"", texPath));
                g_SharedBindables[texPath] = std::make_shared<Texture>(gfx, Surface::FromFile(texPath), 2);
                Log::Debug(std::format("Normals texture \"{}\" was loaded", texPath));
            }
            else
            {
                Log::Debug(std::format("Normals texture \"{}\" was already loaded!", texPath));
            }

            sharedBindables.push_back(g_SharedBindables[texPath]);
        }
    }

    if (hasSpecular)
    {
        if (hasNormals)
        {
            std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, L"PhongVSTex.cso");
            Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
            bindables.push_back(std::move(vShader));

            bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPSTexNorm.cso"));

            bindables.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), vBlob));
        }
        else
        {
            std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, L"PhongVSTex.cso");
            Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
            bindables.push_back(std::move(vShader));

            bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPSTex.cso"));

            bindables.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), vBlob));
        }
    }
    else
    {
        std::unique_ptr<VertexShader> vShader = std::make_unique<VertexShader>(gfx, L"PhongVSTex.cso");
        Microsoft::WRL::ComPtr<ID3DBlob> vBlob = vShader->GetBytecode();
        bindables.push_back(std::move(vShader));

        bindables.push_back(std::make_unique<PixelShader>(gfx, L"PhongPSTexNoSpec.cso"));

        bindables.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), vBlob));
    }

    bindables.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, psMaterial, 1));

    return std::make_unique<Mesh>(gfx, std::move(bindables), sharedBindables);
}
