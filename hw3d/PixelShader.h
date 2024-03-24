#pragma once

#include "Bindable.h"
#include <sstream>


class PixelShader : public Bindable
{
public:
	PixelShader(Graphics* pGfx, const std::wstring& shaderPath);
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

