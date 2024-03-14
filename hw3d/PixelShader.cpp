#include "PixelShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(Graphics* pGfx, const wchar_t* shaderPath)
{
	INFOMAN(pGfx);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	GFX_THROW_NOINFO(D3DReadFileToBlob(shaderPath, &blob));

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
