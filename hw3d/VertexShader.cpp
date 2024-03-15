#include "VertexShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(Graphics* pGfx, const wchar_t* shaderPath)
{
	INFOMAN(pGfx);

	if (FAILED(D3DReadFileToBlob(shaderPath, &shaderBlob)))
	{
		size_t bufferSize = lstrlenW(shaderPath) + 100;
		char* buffer = (char*)malloc(bufferSize);
		memset(buffer, 0, bufferSize);
		snprintf(buffer, bufferSize, "Failed to find Vertex Shader named: %ws", shaderPath);
		std::vector<std::string> errorMessage = { buffer };
		free(buffer);
		throw Graphics::InfoException(__LINE__ -3, __FILE__, errorMessage);
	}

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
