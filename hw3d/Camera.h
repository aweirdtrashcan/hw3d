#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
private:
	float r = 20.f;
	float theta = DirectX::XM_PIDIV2;
	float phi = DirectX::XM_PIDIV2;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};

