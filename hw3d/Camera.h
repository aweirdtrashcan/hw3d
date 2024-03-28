#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera() noexcept;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	const DirectX::XMFLOAT3& GetPosition() const noexcept;
private:
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	float travelSpeed = 15.0f;
	float rotationSpeed = 0.004f;
};

