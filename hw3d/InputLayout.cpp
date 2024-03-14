#include "InputLayout.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"

InputLayout::InputLayout(Graphics* pGfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& ied, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode)
{
	INFOMAN(pGfx);

	GFX_THROW_INFO(GetDevice(pGfx)->CreateInputLayout(
		ied.data(),
		static_cast<UINT>(ied.size()),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&inputLayout
	));
}

void InputLayout::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->IASetInputLayout(inputLayout.Get());
}
