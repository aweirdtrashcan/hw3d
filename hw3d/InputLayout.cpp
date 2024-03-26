#include "InputLayout.h"

#include "DxgiInfoManager.h"
#include "Graphics.h"

#include "BindableCodex.h"

InputLayout::InputLayout(Graphics* pGfx, const hw3dexp::VertexLayout& layout, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode)
{
	INFOMAN(pGfx);

	std::vector<D3D11_INPUT_ELEMENT_DESC> d3dLayout = layout.GetD3DLayout();

	GFX_THROW_INFO(GetDevice(pGfx)->CreateInputLayout(
		d3dLayout.data(),
		static_cast<UINT>(d3dLayout.size()),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&inputLayout
	));
}

void InputLayout::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->IASetInputLayout(inputLayout.Get());
}

std::shared_ptr<Bindable> InputLayout::Resolve(Graphics* gfx, const hw3dexp::VertexLayout& layout, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode)
{
	return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderBytecode);
}

std::string InputLayout::GenerateUID(const hw3dexp::VertexLayout& layout, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode)
{
	return std::format("{}#{}", typeid(InputLayout).name(), layout.GetCode());
}
