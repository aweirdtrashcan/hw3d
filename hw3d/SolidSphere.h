#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class Graphics;

class SolidSphere : public Drawable
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
		float padding = 1.0f;
	} colorConst;
	DirectX::XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	PixelConstantBuffer<PSColorConstant>* cBuf = nullptr;
};
