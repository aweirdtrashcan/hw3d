#include "VertexShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(Graphics* pGfx, const wchar_t* shaderPath)
{
	INFOMAN(pGfx);

	GFX_THROW_NOINFO(D3DReadFileToBlob(shaderPath, &shaderBlob));

	GFX_THROW_INFO(GetDevice(pGfx)->CreateVertexShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		nullptr,
		&vertexShader
	));
}

void VertexShader::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->VSSetShader(vertexShader.Get(), nullptr, 0);
}

Microsoft::WRL::ComPtr<ID3DBlob> VertexShader::GetBytecode() const noexcept
{
	return shaderBlob;
}
