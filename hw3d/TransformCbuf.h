#pragma once

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
	TransformCbuf(Graphics* pGfx, const Drawable& parent, const std::string& tag, UINT slot = 0);
	~TransformCbuf();
	virtual void Bind(Graphics* pGfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* pGfx, const Drawable& parent, const std::string& tag, UINT slot = 0);
	static std::string GenerateUID(const Drawable& parent, const std::string& tag, UINT slot = 0);

private:
	// Bindable = VertexPixelConstantBuffer<Transforms>
	static std::shared_ptr<Bindable> vcbuf;
	const Drawable& parent;
};

