#pragma once

#include <DirectXMath.h>

struct alignas(16) PSMaterialConstant
{
	DirectX::XMFLOAT3 diffuseColor = { 1.0f, 1.0f, 1.0f };
	float specularIntensity = 0.18f;
	float specularPower = 1.0f;
	float shininess = 0.1f;
	int hasGloss = false;
	float p[1];
};