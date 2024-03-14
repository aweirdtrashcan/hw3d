#pragma once

#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics* pGfx, const wchar_t* shaderPath);
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

