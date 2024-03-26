#include "VertexShader.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include <format>
#include "BindableCodex.h"

VertexShader::VertexShader(Graphics* pGfx, const std::string& shaderPath)
{
	INFOMAN(pGfx);

	std::string fullShaderPath = std::format("Shaders\\{}", shaderPath);

	if (FAILED(D3DReadFileToBlob(std::wstring{ fullShaderPath.begin(), fullShaderPath.end() }.c_str(), &shaderBlob)))
	{
		std::stringstream ss;
		ss << "Failed to load Vertex Shader " << std::string(shaderPath.begin(), shaderPath.end());
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

std::shared_ptr<Bindable> VertexShader::Resolve(Graphics* gfx, const std::string& shaderPath)
{
	return Codex::Resolve<VertexShader>(gfx, shaderPath);
}

std::string VertexShader::GenerateUID(const std::string& shaderPath)
{
	return std::format("{}#{}", typeid(VertexShader).name(), shaderPath);
}
