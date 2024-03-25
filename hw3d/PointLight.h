#pragma once

#include <DirectXMath.h>
#include "ConstantBuffers.h"
#include "Camera.h"

class Graphics;
class Drawable;

class PointLight
{
public:
	PointLight(Graphics* gfx, const Camera& camera, float radius = 3.0f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(Graphics* gfx) const noexcept(!_DEBUG);
	void Bind(Graphics* gfx) const noexcept;
private:
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 lightPos = { 0.0f, 100.0f, 10.0f };
		float diffuseIntensity = 1.0f;
		DirectX::XMFLOAT3 ambientColor = { 0.1f, 0.1f, 0.1f };
		float attConst = 0.4f;
		DirectX::XMFLOAT3 lightColor = { 1.0f, 1.0f, 1.0f };
		float attLin = 0.005f;
		DirectX::XMFLOAT3 eyePos = {};
		float attQuad = 0.000f;
	};

	Drawable* mesh;
	const Camera& camera;
	mutable PixelConstantBuffer<PointLightCBuf> cbGpu;
	mutable PointLightCBuf cbCpu = {};
};

