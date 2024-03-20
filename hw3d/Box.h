#pragma once
#include "DrawableBase.h"
#include "ShaderStructs.h"
#include "TestObject.h"

class Box : public TestObject<Box>
{
public:
	Box(Graphics* pGfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material);
	bool SpawnControlWindow(Graphics* gfx) noexcept;
	void SetId(int id) noexcept { this->id = id; }
	int GetId() const noexcept { return id; }
private:
	void SyncMaterial(Graphics* gfx) noexcept(!_DEBUG);
private:
	PSMaterialConstant materialConstants;
	int id;
};