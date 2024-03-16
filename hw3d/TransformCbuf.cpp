#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics* pGfx, const Drawable& parent)
	:
	parent(parent)
{
	if (!vcbuf)
	{
		vcbuf = new VertexConstantBuffer<Transforms>(pGfx);
	}
}

TransformCbuf::~TransformCbuf()
{
	if (vcbuf) 
	{
		delete vcbuf;
		vcbuf = 0;
	}
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

	DirectX::XMStoreFloat4x4(&transforms.modelView, DirectX::XMMatrixTranspose(model ));

	vcbuf->Update(
		pGfx,
		transforms
	);
	vcbuf->Bind(pGfx);
}
