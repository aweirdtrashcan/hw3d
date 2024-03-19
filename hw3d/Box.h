#pragma once
#include "DrawableBase.h"
#include "ShaderStructs.h"

class Box : public DrawableBase<Box>
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
	void Update(float dt) noexcept override;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
	bool SpawnControlWindow(Graphics* gfx) noexcept;
	void SetId(int id) noexcept { this->id = id; }
	int GetId() const noexcept { return id; }
private:
	void SyncMaterial(Graphics* gfx) noexcept(!_DEBUG);
private:
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
	DirectX::XMFLOAT3X3 mt;
	PSMaterialConstant materialConstants;
	int id;
};