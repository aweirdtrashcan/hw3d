#pragma once

#include "Bindable.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	VertexBuffer(Graphics* pGfx, const hw3dexp::VertexBuffer& vbuf, const std::string& tag);
	virtual void Bind(Graphics* pGfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* pGfx, const hw3dexp::VertexBuffer& vbuf, const std::string& tag);
	static std::string GenerateUID(const hw3dexp::VertexBuffer& vbuf, const std::string& tag);

private:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};