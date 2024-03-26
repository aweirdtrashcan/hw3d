#include "PixelShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include <format>

#include "BindableCodex.h"

PixelShader::PixelShader(Graphics* pGfx, std::string_view shaderPath)
{
	INFOMAN(pGfx);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	std::string fullShaderPath = std::format("Shaders\\{}", shaderPath.data());

	if (FAILED(D3DReadFileToBlob(std::wstring{ fullShaderPath.begin(), fullShaderPath.end() }.c_str(), &blob)))
	{
		std::stringstream ss;
		ss << "Failed to load Pixel Shader " << std::string(shaderPath.begin(), shaderPath.end());
		throw Graphics::InfoException(__LINE__ - 3, __FILE__, { ss.str() });
	}

	GFX_THROW_INFO(GetDevice(pGfx)->CreatePixelShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&pixelShader
	));
}

void PixelShader::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->PSSetShader(pixelShader.Get(), nullptr, 0);
}

std::shared_ptr<Bindable> PixelShader::Resolve(Graphics* gfx, std::string_view shaderPath)
{
	return Codex::Resolve<PixelShader>(gfx, shaderPath);
}

std::string PixelShader::GenerateUID(std::string_view shaderPath)
{
	return std::format("{}#{}", typeid(PixelShader).name(), shaderPath);
}
