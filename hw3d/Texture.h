#pragma once

#include "BindableBase.h"

class Texture : public Bindable
{
public:
	Texture(class Graphics* gfx, const class Surface* surface);
	virtual void Bind(Graphics* gfx) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
};