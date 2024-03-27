#include "Texture.h"
#include "Surface.h"
#include "BindableCodex.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(Graphics* gfx, const std::string& path, UINT bindSlot)
	:
	bindSlot(bindSlot)
{
	INFOMAN(gfx);

	TextureImage image(path);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = image.GetImageFormat();
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Height = image.GetHeight();
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = image.GetWidth();

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = image.GetImageRawData();
	srd.SysMemPitch = image.GetImagePitch();

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

// Image
Texture::TextureImage::TextureImage(const std::string& path)
	:
	path(path),
	imageData(nullptr),
	width(0),
	height(0),
	channels(0),
	converted(false)
{
	imageData = stbi_load(path.c_str(), &width, &height, &channels, 0);
	ConvertPng();
}

Texture::TextureImage::~TextureImage()
{
	if (imageData)
	{
		if (converted)
		{
			delete[] imageData;
		}
		else
		{
			stbi_image_free(imageData);
		}
	}
}

DXGI_FORMAT Texture::TextureImage::GetImageFormat() const noexcept(!IS_DEBUG)
{
	switch (channels)
	{
	case 1:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case 2:
		throw std::exception("Unknown format");
	case 3:
		throw std::exception("Unknown format");
	case 4:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	throw std::exception("Unknown format");
}

UINT Texture::TextureImage::GetImagePitch() const noexcept
{
	return (UINT)width * channels;
}

UINT Texture::TextureImage::GetWidth() const noexcept
{
	return (UINT)width;
}

UINT Texture::TextureImage::GetHeight() const noexcept
{
	return (UINT)height;
}

const void* Texture::TextureImage::GetImageRawData() const noexcept
{
	return imageData;
}

void Texture::TextureImage::ConvertPng() noexcept(!IS_DEBUG)
{
	if (channels != 3)
	{
		converted = false;
		return;
	}

	struct JPGColor
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	struct PNGColor
	{
		PNGColor(JPGColor* pc)
			:
			r(pc->r),
			g(pc->g),
			b(pc->b),
			a(0)
		{}
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	size_t pngSize = width * height * sizeof(PNGColor);
	unsigned char* png = new unsigned char[pngSize];

	size_t pixelColorIndex = 0;

	for (int h = 0; h < height * 3; h += 3)
	{
		for (int w = 0; w < width * 3; w += 3)
		{
			size_t colorIndex = (h * height) + w;
			JPGColor* jc = (JPGColor*)&imageData[colorIndex];
			PNGColor pc(jc);
			memcpy(&png[pixelColorIndex], &pc, sizeof(pc));
			pixelColorIndex += 4;
		}
	}

	channels = 4;
	converted = true;

	stbi_image_free(imageData);
	
	imageData = png;
}
