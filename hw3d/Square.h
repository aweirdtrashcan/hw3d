#pragma once

#include "Drawable.h"

class Square : public Drawable
{
public:
	Square(Graphics* gfx, float scaleFactor);

	// Inherited via Drawable
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void Update(float dt) noexcept override;
private:
	DirectX::XMFLOAT4X4 model;
};

