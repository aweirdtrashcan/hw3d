#include "Square.h"

#include "Vertex.h"
#include "BindableCodex.h"
#include "BindableBase.h"
#include "ShaderStructs.h"

Square::Square(Graphics* gfx, float scaleFactor)
{
	DirectX::XMStoreFloat4x4(&model, DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor));
	
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	hw3dexp::VertexLayout vLayout = hw3dexp::VertexLayout()
		.Append(hw3dexp::VertexLayout::Position3D)
		.Append(hw3dexp::VertexLayout::Normal)
		.Append(hw3dexp::VertexLayout::Texture2D);

	hw3dexp::VertexBuffer vBuffer(vLayout);

	struct SVertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
		DirectX::XMFLOAT2 tex;
	};

	std::vector<SVertex> vertices =
	{
		{ { -0.5f,	0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,	0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	};

	std::vector<unsigned int> indices =
	{
		0, 1, 2,
		2, 1, 3
	};

	for (const SVertex& v : vertices)
	{
		vBuffer.EmplaceBack(v.pos, v.n, v.tex);
	}

	AddBind(VertexBuffer::Resolve(gfx, vBuffer, "square_vb"));
	AddBind(IndexBuffer::Resolve(gfx, indices, "square_ib"));

	std::shared_ptr<Bindable> vShader = VertexShader::Resolve(gfx, "PhongVSTex.cso");
	Microsoft::WRL::ComPtr<ID3DBlob> vBlob = std::static_pointer_cast<VertexShader>(vShader)->GetBytecode();

	AddBind(InputLayout::Resolve(gfx, vLayout, vBlob));

	AddBind(vShader);
	AddBind(PixelShader::Resolve(gfx, "PhongPSTexNoSpec.cso"));
	
	AddBind(Sampler::Resolve(gfx));

	PSMaterialConstant mat{};
	mat.diffuseColor = { 1.0f,1.0f,1.0f };
	
	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, mat, 1));
	
	AddBind(Texture::Resolve(gfx, "C:/Users/Diego/source/repos/hw3d/hw3d/Models/brick/Brick_Wall_018_basecolor.jpg", 0));
	AddBind(Texture::Resolve(gfx, "C:/Users/Diego/source/repos/hw3d/hw3d/Models/brick/Brick_Wall_018_normal.jpg", 2));

	AddBind(TransformCbuf::Resolve(gfx, *this, "square_tcb", 2));
}

DirectX::XMMATRIX Square::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&model);
}

void Square::Update(float dt) noexcept
{
}
