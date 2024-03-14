#include "IndexBuffer.h"

#include "Macros.h"
#include "DxgiInfoManager.h"
#include "Graphics.h"

IndexBuffer::IndexBuffer(Graphics* pGfx, const std::vector<unsigned short>& indices)
	:
	count((UINT)indices.size())
{
	INFOMAN(pGfx);

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(count * sizeof(unsigned short));
	desc.StructureByteStride = sizeof(unsigned short);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = indices.data();

	GFX_THROW_INFO(GetDevice(pGfx)->CreateBuffer(&desc, &srd, &buffer));
}

void IndexBuffer::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}
