#include "PointLight.h"

#include "imgui.h"
#include "Drawable.h"
#include "SolidSphere.h"

PointLight::PointLight(Graphics* gfx, const Camera& camera, float radius)
	:
	mesh(new SolidSphere(gfx, radius)),
	cbGpu(gfx, 0),
	camera(camera)
{}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbCpu.lightPos.x, -60.f, 60.f, "%.1f");
		ImGui::SliderFloat("Y", &cbCpu.lightPos.y, -60.f, 60.f, "%.1f");
		ImGui::SliderFloat("Z", &cbCpu.lightPos.z, -60.f, 60.f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}

		ImGui::Text("Diffuse");
		ImGui::ColorEdit3("Diffuse Color", &cbCpu.lightColor.x);
		ImGui::SliderFloat("Diffuse Intensity", &cbCpu.diffuseIntensity, 0.0f, 1.0f);
		ImGui::Text("Attenuation");
		ImGui::SliderFloat("Constant Attenuation", &cbCpu.attConst, 0.0f, 10.0f);
		ImGui::SliderFloat("Linear Attenuation", &cbCpu.attLin, 0.0f, 0.05f);
		ImGui::SliderFloat("Quadratic Attenuation", &cbCpu.attQuad, 0.0f, 0.001f);
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbCpu = PointLightCBuf();
}

void PointLight::Draw(Graphics* gfx) const noexcept(!_DEBUG)
{
	mesh->Draw(gfx);
}

void PointLight::Bind(Graphics* gfx) const noexcept
{
	static_cast<SolidSphere*>(mesh)->SetPos(cbCpu.lightPos);
	static_cast<SolidSphere*>(mesh)->SetColor(gfx, cbCpu.lightColor);
	cbGpu.Update(gfx, cbCpu);
	cbGpu.Bind(gfx);

	const DirectX::XMVECTOR& cameraPos = camera.GetPosition();
	cbCpu.eyePos.x = DirectX::XMVectorGetX(cameraPos);
	cbCpu.eyePos.y = DirectX::XMVectorGetY(cameraPos);
	cbCpu.eyePos.z = DirectX::XMVectorGetZ(cameraPos);
}
