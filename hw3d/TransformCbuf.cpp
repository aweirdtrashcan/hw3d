#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics* pGfx, const Drawable& parent)
	:
	parent(parent)
{
	if (!vcbuf)
	{
		vcbuf = new VertexConstantBuffer<DirectX::XMFLOAT4X4>(pGfx);
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
	DirectX::XMMATRIX m = DirectX::XMMatrixTranspose(parent.GetTransformXM() * pGfx->GetView() * pGfx->GetProjection());
	DirectX::XMFLOAT4X4 mvp;
	DirectX::XMStoreFloat4x4(&mvp, m);
	vcbuf->Update(
		pGfx,
		mvp
	);
	vcbuf->Bind(pGfx);
}
