#include "Camera.h"

#include "imgui.h"

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	
	DirectX::XMVECTOR pos = GetPosition();
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX cameraMat = DirectX::XMMatrixLookAtLH(pos, target, up) *
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	return cameraMat;
}

DirectX::XMVECTOR Camera::GetPosition() const noexcept
{
	float x = r * cosf(theta) * sinf(phi);
	float y = r * cosf(phi);
	float z = r * sinf(theta) * sinf(phi);

	return DirectX::XMVectorSet(x, y, z, 1.0f);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 1.0f, 80.f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, 0.0f, 360.f);
		ImGui::SliderAngle("Phi", &phi, 1.0f, 179.f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, 0.0f, 180.f);
		ImGui::SliderAngle("Roll", &roll, 0.0f, 180.f);
		ImGui::SliderAngle("Yaw", &yaw, 0.0f, 180.f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.f;
	theta = DirectX::XM_PIDIV2;
	phi = DirectX::XM_PIDIV2;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}
