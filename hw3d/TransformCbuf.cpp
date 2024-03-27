#include "TransformCbuf.h"

#include "BindableCodex.h"

std::shared_ptr<Bindable> TransformCbuf::vcbuf;

TransformCbuf::TransformCbuf(Graphics* pGfx, const Drawable& parent, const std::string& tag, UINT slot)
	:
	parent(parent)
{
	using VPCB = VertexPixelConstantBuffer<Transforms>;
	vcbuf = std::static_pointer_cast<VPCB>( VPCB::Resolve(pGfx, slot));
}

TransformCbuf::~TransformCbuf()
{
}

void TransformCbuf::Bind(Graphics* pGfx) noexcept
{
	Transforms transforms;

	const DirectX::XMMATRIX& model = parent.GetTransformXM();
	const DirectX::XMMATRIX& view = pGfx->GetView();

	DirectX::XMStoreFloat4x4(&transforms.modelViewProj, 
		DirectX::XMMatrixTranspose(
			model *
			view *
			pGfx->GetProjection()
		)
	);

	DirectX::XMStoreFloat4x4(&transforms.model, DirectX::XMMatrixTranspose(model));
	DirectX::XMStoreFloat4x4(&transforms.modelView, DirectX::XMMatrixTranspose(model * view));

	using VPCB = VertexPixelConstantBuffer<Transforms>;
	VPCB* tcb = (VPCB*)vcbuf.get();
	tcb->Update(
		pGfx,
		transforms
	);
	tcb->Bind(pGfx);
}

std::shared_ptr<Bindable> TransformCbuf::Resolve(Graphics* pGfx, const Drawable& parent, const std::string& tag, UINT slot)
{
	return Codex::Resolve<TransformCbuf>(pGfx, parent, tag, slot);
}

std::string TransformCbuf::GenerateUID(const Drawable& parent, const std::string& tag, UINT slot)
{
	return std::format("{}#{}T{}", typeid(TransformCbuf).name(), slot, tag);
}
