#pragma once

#include "Bindable.h"
#include <sstream>

class PixelShader : public Bindable
{
public:
	PixelShader(Graphics* pGfx, std::string_view shaderPath);
	virtual void Bind(Graphics* pGfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, std::string_view shaderPath);
	static std::string GenerateUID(std::string_view shaderPath);

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

