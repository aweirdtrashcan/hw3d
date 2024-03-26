#include "TransformCbuf.h"

#include "BindableCodex.h"

TransformCbuf::TransformCbuf(Graphics* pGfx, const Drawable& parent, const std::string& tag, UINT slot)
	:
	parent(parent)
{
	using TCB = TransformConstantBuffer<Transforms>;
	vcbuf = TCB::Resolve(pGfx, slot);
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

	using TCB = TransformConstantBuffer<Transforms>;
	std::static_pointer_cast<TCB>(vcbuf)->Update(
		pGfx,
		transforms
	);
	vcbuf->Bind(pGfx);
}

std::shared_ptr<Bindable> TransformCbuf::Resolve(Graphics* pGfx, const Drawable& parent, const std::string& tag, UINT slot)
{
	return Codex::Resolve<TransformCbuf>(pGfx, parent, tag, slot);
}

std::string TransformCbuf::GenerateUID(const Drawable& parent, const std::string& tag, UINT slot)
{
	return std::format("{}#{}T{}", typeid(TransformCbuf).name(), slot, tag);
}
