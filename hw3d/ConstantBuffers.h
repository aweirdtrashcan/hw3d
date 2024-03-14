#pragma once

#include "Bindable.h"

#include "Graphics.h"
#include "DxgiInfoManager.h"
#include <string>

template<typename C>
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics* pGfx, const C& data)
	{
		D3D11_MAPPED_SUBRESOURCE msr = {};
		INFOMAN(pGfx);

		GFX_THROW_INFO(GetContext(pGfx)->Map(
			buffer.Get(),
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&msr
		));

		memcpy(msr.pData, &data, sizeof(data));

		GetContext(pGfx)->Unmap(buffer.Get(), 0);
	}

	ConstantBuffer(Graphics* pGfx, const C& data)
	{
		INFOMAN(pGfx);

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(C);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		
		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = &data;

		GFX_THROW_INFO(GetDevice(pGfx)->CreateBuffer(&desc, &srd, &buffer));
	}
	ConstantBuffer(Graphics* pGfx)
	{
		INFOMAN(pGfx);

		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(C);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GFX_THROW_INFO(GetDevice(pGfx)->CreateBuffer(&desc, nullptr, &buffer));
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetContext;
	using ConstantBuffer<C>::buffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	virtual void Bind(Graphics* pGfx) noexcept override
	{
		GetContext(pGfx)->PSSetConstantBuffers(0, 1, buffer.GetAddressOf());
	}
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetContext;
	using ConstantBuffer<C>::buffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	virtual void Bind(Graphics* pGfx) noexcept override
	{
		GetContext(pGfx)->VSSetConstantBuffers(0, 1, buffer.GetAddressOf());
	}
};