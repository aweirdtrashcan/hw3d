#pragma once

#include "Bindable.h"
#include "Vertex.h"

class InputLayout : public Bindable
{
public:
	InputLayout(Graphics* pGfx, const hw3dexp::VertexLayout& layout, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode);
	virtual ~InputLayout() = default;
	virtual void Bind(Graphics* pGfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* pGfx, const hw3dexp::VertexLayout& layout, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode);
	static std::string GenerateUID(const hw3dexp::VertexLayout& layout, Microsoft::WRL::ComPtr<ID3DBlob> pVertexShaderBytecode);
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

