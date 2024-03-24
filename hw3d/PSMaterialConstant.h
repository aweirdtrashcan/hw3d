#pragma once

#include <DirectXMath.h>

struct alignas(16) PSMaterialConstant
{
	DirectX::XMFLOAT3 albedoColor = {};
	float specularIntensity = 0.6f;
	DirectX::XMFLOAT3 fresnelR0 = { 0.95f, 0.93f, 0.88f };
	float specularPower = 1.0f;
	DirectX::XMFLOAT3 ambient = { 0.0f, 0.0f, 0.0f };
	float shininess = 0.1f;
};