#pragma once

#include "BindableBase.h"

class Sampler : public Bindable
{
public:
	Sampler(class Graphics* gfx);
	virtual void Bind(Graphics* gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx);
	static std::string GenerateUID();
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};