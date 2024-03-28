#include "Camera.h"

#include "imgui.h"
#include <algorithm>

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	DirectX::XMVECTOR forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR lookVector = DirectX::XMVector3Transform(
		forwardBaseVector,
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0)
	);

	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&this->pos);
	DirectX::XMVECTOR target = DirectX::XMVectorAdd(pos, lookVector);
	return DirectX::XMMatrixLookAtLH(pos, target, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.f, 80.f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.f, 80.f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.f, 80.f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -90.f, 90.f);
		ImGui::SliderAngle("Yaw", &yaw, -90.f, 90.f);
		ImGui::Text("Camera Settings");
		ImGui::SliderFloat("Move Speed", &travelSpeed, 0.0f, 300.f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos.x = 0.0f;
	pos.y = 7.5f;
	pos.z = -18.f;
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = (yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, -DirectX::XM_PIDIV2 * 0.995f, DirectX::XM_PIDIV2 * 0.995f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation, DirectX::XMVector3Transform(
		DirectX::XMLoadFloat3(&translation),
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
		DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
	));

	pos.x += translation.x;
	pos.y += translation.y;
	pos.z += translation.z;
}

const DirectX::XMFLOAT3& Camera::GetPosition() const noexcept
{
	return pos;
}
