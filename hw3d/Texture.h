#pragma once

#include "BindableBase.h"
#include "Surface.h"

class Texture : public Bindable
{
public:
	Texture(class Graphics* gfx, Surface&& surface, UINT bindSlot = 0);
	virtual void Bind(Graphics* gfx) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	UINT bindSlot;
};