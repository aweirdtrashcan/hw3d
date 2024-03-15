#include "PixelShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(Graphics* pGfx, const wchar_t* shaderPath)
{
	INFOMAN(pGfx);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	if (FAILED(D3DReadFileToBlob(shaderPath, &blob)))
	{
		size_t bufferSize = lstrlenW(shaderPath) + 100;
		char* buffer = (char*)malloc(bufferSize);
		memset(buffer, 0, bufferSize);
		snprintf(buffer, bufferSize, "Failed to find Pixel Shader named: %ws", shaderPath);
		std::vector<std::string> errorMessage = { buffer };
		free(buffer);
		throw Graphics::InfoException(__LINE__ - 3, __FILE__, errorMessage);
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
