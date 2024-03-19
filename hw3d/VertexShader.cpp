#include "VertexShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include <sstream>
#include <sstream>

VertexShader::VertexShader(Graphics* pGfx, const wchar_t* shaderPath)
{
	INFOMAN(pGfx);

	std::wstringstream wss;
	wss << L"Shaders\\" << shaderPath;

	if (FAILED(D3DReadFileToBlob(wss.str().c_str(), &shaderBlob)))
	{
		std::stringstream ss;
		ss << "Failed to load Vertex Shader " << shaderPath;
		throw Graphics::InfoException(__LINE__ -3, __FILE__, { ss.str()});
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
