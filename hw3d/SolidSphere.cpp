#include "SolidSphere.h"
#include "BindableBase.h"
#include "Macros.h"
#include "Sphere.h"
#include "BindableCodex.h"
#include "ShaderStructs.h"

SolidSphere::SolidSphere(Graphics* gfx, float radius, UINT bindSlot)
{
	namespace dx = DirectX;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};
	auto model = Sphere::Make<Vertex>();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));

	hw3dexp::VertexLayout vLayout = hw3dexp::VertexLayout();
	vLayout.Append(hw3dexp::VertexLayout::Position3D);
	hw3dexp::VertexBuffer dynVB = hw3dexp::VertexBuffer(vLayout);

	for (auto m : model.vertices)
	{
		dynVB.EmplaceBack(m.pos);
	}

	AddBind(VertexBuffer::Resolve(gfx, dynVB, "solid_sphere_vb"));
	AddBind(IndexBuffer::Resolve(gfx, model.indices, "solid_sphere_ib"));

	std::shared_ptr<Bindable> pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	Microsoft::WRL::ComPtr<ID3DBlob> pvsbc = std::static_pointer_cast<VertexShader>(pvs)->GetBytecode();
	AddBind(pvs);

	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	using PSConstBuffer = PixelConstantBuffer<PSColorConstant>;
	std::shared_ptr<PSConstBuffer> pCb = std::static_pointer_cast<PSConstBuffer>(PSConstBuffer::Resolve(gfx, colorConst, bindSlot));
	cBuf = pCb.get();

	AddBind(pCb);

	AddBind(InputLayout::Resolve(gfx, vLayout, pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(TransformCbuf::Resolve(gfx, *this, "solid_sphere_transformcbuf", 2));
	

}

void SolidSphere::Update(float dt) noexcept
{
}

void SolidSphere::SetPos(const DirectX::XMFLOAT3& pos) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void SolidSphere::SetColor(Graphics* gfx, const DirectX::XMFLOAT3& color) noexcept
{
	if (cBuf)
	{
		colorConst.color = color;
		cBuf->Update(gfx, colorConst);
	}
}
