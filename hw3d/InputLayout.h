#pragma once

#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics* pGfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& ied, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode);
	virtual ~InputLayout() = default;
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

