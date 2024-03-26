#pragma once

#include <vector>
#include <d3d11.h>
#include <wrl.h>
#include "Macros.h"
#include <string>
#include <memory>

class Graphics;

class Bindable
{
public:
	virtual void Bind(Graphics* pGfx) noexcept = 0;
	virtual ~Bindable() = default;
	std::string GetUID() const noexcept { return uid; }
	void SetUID(std::string uid) noexcept { this->uid = uid; }

protected:
	static struct ID3D11DeviceContext* GetContext(Graphics* pGfx) noexcept;
	static struct ID3D11Device* GetDevice(Graphics* pGfx) noexcept;
	static class DxgiInfoManager& GetInfoManager(Graphics* pGfx) noexcept(!_DEBUG);
	mutable std::string uid;
};