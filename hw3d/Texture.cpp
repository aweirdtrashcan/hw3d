#include "Texture.h"
#include "Surface.h"
#include "BindableCodex.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* fromJpgToPng(unsigned char* jpg, int width, int height, int* channels, bool* converted)
{
	if (*channels != 3)
	{
		*converted = false;
		return jpg;
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
	unsigned char* png = (unsigned char*)malloc(pngSize);
	memset(png, 0, pngSize);

	size_t pixelColorIndex = 0;

	for (unsigned int h = 0; h < height * 3; h+=3)
	{
		for (unsigned int w = 0; w < width * 3; w += 3)
		{
			size_t colorIndex = (h * height) + w;
			JPGColor* jc = (JPGColor*)&jpg[colorIndex];
			PNGColor pc(jc);
			memcpy(&png[pixelColorIndex], &pc, sizeof(pc));
			pixelColorIndex += 4;
		}
	}

	*channels = 4;
	*converted = true;

	stbi_image_free(jpg);

	return png;
}

Texture::Texture(Graphics* gfx, const std::string& path, UINT bindSlot)
	:
	bindSlot(bindSlot)
{
	INFOMAN(gfx);

	int width = 0, height = 0, channels = 0;
	bool converted = false;
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 0);
	image = fromJpgToPng(image, width, height, &channels, &converted);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = width;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = image;
	srd.SysMemPitch = width * channels;

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

	if (converted)
		free(image);
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
