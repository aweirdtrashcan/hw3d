#pragma once

#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics* pGfx, const wchar_t* shaderPath);
	virtual void Bind(Graphics* pGfx) noexcept override;
	Microsoft::WRL::ComPtr<ID3DBlob> GetBytecode() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
};