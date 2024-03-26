#pragma once

#include "BindableBase.h"
#include "Surface.h"

class Texture : public Bindable
{
public:
	Texture(class Graphics* gfx, const std::string& path, UINT bindSlot = 0);
	virtual void Bind(Graphics* gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, const std::string& path, UINT bindSlot = 0);
	static std::string GenerateUID(const std::string& path, UINT bindSlot = 0);
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	UINT bindSlot;
};