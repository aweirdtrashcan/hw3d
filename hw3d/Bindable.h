#pragma once

#include <vector>
#include <d3d11.h>
#include <wrl.h>
#include "Macros.h"

class Graphics;

class Bindable
{
public:
	virtual void Bind(Graphics* pGfx) noexcept = 0;
	virtual ~Bindable() = default;

protected:
	static struct ID3D11DeviceContext* GetContext(Graphics* pGfx) noexcept;
	static struct ID3D11Device* GetDevice(Graphics* pGfx) noexcept;
	static class DxgiInfoManager& GetInfoManager(Graphics* pGfx) noexcept(!_DEBUG);
};