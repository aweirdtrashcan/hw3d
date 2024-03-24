#include "Box.h"
#include "BindableBase.h"
#include "Macros.h"
#include "Cube.h"
#include "imgui.h"


Box::Box(Graphics* gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist, bdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};
		IndexedTriangleList<Vertex> model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}


	materialConstants.albedoColor = material;

	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, materialConstants, 1));

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	// model deformation transform (per instance, not stored as bind)
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

bool Box::SpawnControlWindow(Graphics* gfx) noexcept
{
	bool hasValuesChanged = false;
	bool open = true;

	char* str = (char*)alloca(50);

	snprintf(str, 50, "Box %d\n\0", id);

	if (ImGui::Begin(str, &open))
	{
		ImGui::Text("Material Properties");
	 	bool albedoChanged = ImGui::ColorEdit3("Material Albedo", &materialConstants.albedoColor.x);
		bool fresnelChanged = ImGui::SliderFloat3("Fresnel R0", &materialConstants.fresnelR0.x, 0.0f, 1.0f);
		bool specIntChanged = ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity, 0.0f, 1.0f);
		bool specPwrChanged = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 0.0f, 1.0f);
		bool roughChanged = ImGui::SliderFloat("Roughness", &materialConstants.shininess, 0.0f, 1.0f);
		hasValuesChanged = albedoChanged || fresnelChanged || specIntChanged || specPwrChanged || roughChanged;

		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.f);
		ImGui::SliderAngle("Theta", &theta, -180.f, 180.f);
		ImGui::SliderAngle("Phi", &phi, -180.f, 180.f);
	}
	ImGui::End();

	if (hasValuesChanged)
	{
		SyncMaterial(gfx);
	}

	return open;
}

void Box::SyncMaterial(Graphics* gfx) noexcept(!_DEBUG)
{
	using PCB = PixelConstantBuffer<PSMaterialConstant>;
	PCB* pCBuf = QueryBindable<PCB>();
	if (pCBuf)
	{
		pCBuf->Update(gfx, materialConstants);
	}
}
