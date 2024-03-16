#pragma once

#include "DrawableBase.h"

class Graphics;

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(Graphics* gfx, float radius);
	virtual void Update(float dt) noexcept override;
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
};
