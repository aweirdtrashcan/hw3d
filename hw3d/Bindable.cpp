#include "Bindable.h"

#include <d3d11.h>
#include <wrl.h>
#include "Graphics.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics* pGfx) noexcept
{
	return pGfx->pContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics* pGfx) noexcept
{
	return pGfx->pDevice.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(Graphics* pGfx) noexcept(!_DEBUG)
{
	return pGfx->infoManager;
}
