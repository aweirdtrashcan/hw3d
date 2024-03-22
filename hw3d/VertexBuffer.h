#pragma once

#include "Bindable.h"
#include "Graphics.h"
#include "DxgiInfoManager.h"

#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	template<typename T>
	VertexBuffer(Graphics* pGfx, const std::vector<T>& vertices)
		:
		stride(sizeof(T))
	{
		INFOMAN(pGfx);

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = (UINT)vertices.size() * sizeof(T);
		desc.StructureByteStride = sizeof(T);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		
		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = vertices.data();

		GFX_THROW_INFO(GetDevice(pGfx)->CreateBuffer(&desc, &srd, &buffer));
	}

	VertexBuffer(Graphics* pGfx, const hw3dexp::VertexBuffer& vbuf)
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

	virtual void Bind(Graphics* pGfx) noexcept override
	{
		UINT offset = 0;
		GetContext(pGfx)->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset);
	}
private:
	UINT stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};