#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformCbuf : public Bindable
{
public:
	TransformCbuf(Graphics* pGfx, const Drawable& parent);
	~TransformCbuf();
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	static inline VertexConstantBuffer<DirectX::XMFLOAT4X4>* vcbuf;
	const Drawable& parent;
};

