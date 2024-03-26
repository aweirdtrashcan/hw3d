#pragma once

#include "Bindable.h"
#include <sstream>

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics* pGfx, const std::string& shaderPath);
	virtual void Bind(Graphics* pGfx) noexcept override;
	Microsoft::WRL::ComPtr<ID3DBlob> GetBytecode() const noexcept;
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, const std::string& shaderPath);
	static std::string GenerateUID(const std::string& shaderPath);
private:
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
};