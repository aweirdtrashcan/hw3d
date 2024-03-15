#pragma once

#include "BindableBase.h"

class Sampler : public Bindable
{
public:
	Sampler(class Graphics* gfx);
	virtual void Bind(Graphics* gfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};