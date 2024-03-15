#include "Sampler.h"

Sampler::Sampler(Graphics* gfx)
{
	INFOMAN(gfx);

	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(
		&desc,
		&samplerState
	));
}

void Sampler::Bind(Graphics* gfx) noexcept
{
	GetContext(gfx)->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}
