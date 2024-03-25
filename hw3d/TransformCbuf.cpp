#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics* pGfx, const Drawable& parent, UINT slot)
	:
	parent(parent)
{
	vcbuf = std::make_unique<TransformConstantBuffer<Transforms>>(pGfx, slot);
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

	vcbuf->Update(
		pGfx,
		transforms
	);
	vcbuf->Bind(pGfx);
}
