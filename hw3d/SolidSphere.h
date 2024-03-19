#pragma once

#include "DrawableBase.h"
#include "ConstantBuffers.h"

class Graphics;

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(Graphics* gfx, float radius);
	virtual void Update(float dt) noexcept override;
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void SetColor(Graphics* gfx, const DirectX::XMFLOAT3& color) noexcept;
private:

	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	PixelConstantBuffer<PSColorConstant>* cBuf = nullptr;
};
