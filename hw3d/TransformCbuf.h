#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformCbuf : public Bindable
{
private:
	struct Transforms
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 modelView;
		DirectX::XMFLOAT4X4 modelViewProj;
	};
public:
	TransformCbuf(Graphics* pGfx, const Drawable& parent, UINT slot = 0);
	~TransformCbuf();
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	static inline VertexConstantBuffer<Transforms>* vcbuf;
	const Drawable& parent;
};

