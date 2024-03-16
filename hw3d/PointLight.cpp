#include "PointLight.h"

#include "imgui.h"
#include "Drawable.h"
#include "SolidSphere.h"

PointLight::PointLight(Graphics* gfx, float radius)
	:
	mesh(new SolidSphere(gfx, radius)),
	cbuf(gfx)
{
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -60.f, 60.f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -60.f, 60.f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -60.f, 60.f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	pos = { 0.0f, 0.0f, 0.0f };
}

void PointLight::Draw(Graphics* gfx) const noexcept(!_DEBUG)
{
	mesh->Draw(gfx);
}

void PointLight::Bind(Graphics* gfx) const noexcept
{
	static_cast<SolidSphere*>(mesh)->SetPos(pos);
	cbuf.Update(gfx, PointLightCBuf{ pos });
	cbuf.Bind(gfx);
}
