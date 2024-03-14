#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformCbuf : public Bindable
{
public:
	TransformCbuf(Graphics* pGfx, const Drawable& parent);
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	VertexConstantBuffer<DirectX::XMFLOAT4X4> vcbuf;
	const Drawable& parent;
};

