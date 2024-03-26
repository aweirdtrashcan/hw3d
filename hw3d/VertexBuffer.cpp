#include "VertexBuffer.h"

#include "BindableCodex.h"
#include "Graphics.h"
#include "DxgiInfoManager.h"

VertexBuffer::VertexBuffer(Graphics* pGfx, const hw3dexp::VertexBuffer& vbuf, const std::string& tag)
	:
	stride((UINT)vbuf.GetLayout().Size())
{
	INFOMAN(pGfx);

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (UINT)vbuf.SizeBytes();
	desc.StructureByteStride = stride;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = vbuf.GetData();

	GFX_THROW_INFO(GetDevice(pGfx)->CreateBuffer(&desc, &srd, &buffer));
}

void VertexBuffer::Bind(Graphics* pGfx) noexcept
{
	UINT offset = 0;
	GetContext(pGfx)->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset);
}

std::shared_ptr<Bindable> VertexBuffer::Resolve(Graphics* pGfx, const hw3dexp::VertexBuffer& vbuf, const std::string& tag)
{
    return Codex::Resolve<VertexBuffer>(pGfx, vbuf, tag);
}

std::string VertexBuffer::GenerateUID(const hw3dexp::VertexBuffer& vbuf, const std::string& tag)
{
    return std::format("{}#{}T{}", typeid(VertexBuffer).name(), vbuf.GetLayout().GetCode(), tag);
}
