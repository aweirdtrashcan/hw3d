#include "IndexBuffer.h"

#include "Macros.h"
#include "DxgiInfoManager.h"
#include "Graphics.h"

#include "BindableCodex.h"

IndexBuffer::IndexBuffer(Graphics* pGfx, const std::vector<unsigned int>& indices, const std::string& tag)
	:
	count((UINT)indices.size())
{
	INFOMAN(pGfx);

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = static_cast<UINT>(count * sizeof(unsigned int));
	desc.StructureByteStride = sizeof(unsigned int);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = indices.data();

	GFX_THROW_INFO(GetDevice(pGfx)->CreateBuffer(&desc, &srd, &buffer));
}

void IndexBuffer::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

std::shared_ptr<Bindable> IndexBuffer::Resolve(Graphics* gfx, const std::vector<unsigned int>& indices, const std::string& tag)
{
	return Codex::Resolve<IndexBuffer>(gfx, indices, tag);
}

std::string IndexBuffer::GenerateUID(const std::vector<unsigned int>& indices, const std::string& tag)
{
	return std::format("{}#T{}", typeid(IndexBuffer).name(), tag);
}
