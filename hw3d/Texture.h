#pragma once

#include "BindableBase.h"
#include "Surface.h"

class Texture : public Bindable
{
protected:
	class TextureImage
	{
	private:
		class TextureImageLoadException : public ChiliException
		{
		public:
			TextureImageLoadException(int line, const char* file, std::string reason) noexcept;
			virtual const char* GetType() const noexcept;
			virtual const char* what() const noexcept;
		};
	public:
		TextureImage(const std::string& path);
		~TextureImage();

		DXGI_FORMAT GetImageFormat() const noexcept(!IS_DEBUG);
		UINT GetImagePitch() const noexcept;
		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
		const void* GetImageRawData() const noexcept;
		bool HasAlpha() const noexcept { return !converted; }
	private:
		bool ConvertPng() noexcept(!IS_DEBUG);
		void ConvertXXXA() noexcept(!IS_DEBUG);
	private:
		std::string path;
		unsigned char* imageData;
		int width;
		int height;
		int channels;
		bool converted;
	};
public:
	Texture(class Graphics* gfx, const std::string& path, UINT bindSlot);
	virtual void Bind(Graphics* gfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, const std::string& path, UINT bindSlot = 0);
	static std::string GenerateUID(const std::string& path, UINT bindSlot);
	bool HasAlpha() const noexcept;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV;
	UINT bindSlot;
	bool bHasAlpha = false;
};