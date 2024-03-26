#include "Texture.h"

#include "Surface.h"

#include "BindableCodex.h"

Texture::Texture(Graphics* gfx, const std::string& path, UINT bindSlot)
	:
	bindSlot(bindSlot)
{
	INFOMAN(gfx);

	Surface surface = Surface::FromFile(path);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Height = surface.GetHeight();
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = surface.GetWidth();

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = surface.GetBufferPtrConst();
	srd.SysMemPitch = surface.GetWidth() * sizeof(Surface::Color);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;

	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
		&desc,
		&srd,
		&pTexture
	));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
		pTexture.Get(),
		&srvDesc,
		&pSRV
	));
}

void Texture::Bind(Graphics* gfx) noexcept
{
	GetContext(gfx)->PSSetShaderResources(bindSlot, 1, pSRV.GetAddressOf());
}

std::shared_ptr<Bindable> Texture::Resolve(Graphics* gfx, const std::string& path, UINT bindSlot)
{
	return Codex::Resolve<Texture>(gfx, path, bindSlot);
}

std::string Texture::GenerateUID(const std::string& path, UINT bindSlot)
{
	return std::format("{}#{}#{}", typeid(Texture).name(), path, bindSlot);
}
