#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics* pGfx, const Drawable& parent)
	:
	vcbuf(pGfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics* pGfx) noexcept
{
	DirectX::XMMATRIX model = parent.GetTransformXM();
	DirectX::XMMATRIX view = pGfx->GetView();
	DirectX::XMMATRIX projection = pGfx->GetProjection();
	DirectX::XMFLOAT4X4 mvp;
	DirectX::XMStoreFloat4x4(&mvp, DirectX::XMMatrixTranspose(projection * view * model));
	vcbuf.Update(
		pGfx,
		mvp
	);
}
