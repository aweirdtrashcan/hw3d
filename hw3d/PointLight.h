#pragma once

#include <DirectXMath.h>
#include "ConstantBuffers.h"

class Graphics;
class Drawable;

class PointLight
{
public:
	PointLight(Graphics* gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(Graphics* gfx) const noexcept(!_DEBUG);
	void Bind(Graphics* gfx) const noexcept;
private:
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};

	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
	mutable Drawable* mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};

