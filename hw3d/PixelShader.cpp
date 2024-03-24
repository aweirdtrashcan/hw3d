#include "PixelShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(Graphics* pGfx, const std::wstring& shaderPath)
{
	INFOMAN(pGfx);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	std::wstringstream wss;

	wss << L"Shaders\\" << shaderPath;

	if (FAILED(D3DReadFileToBlob(wss.str().c_str(), &blob)))
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
