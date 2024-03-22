#pragma once

#include "Bindable.h"

#include <d3d11.h>
#include <wrl.h>
#include <vector>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics* pGfx, const std::vector<unsigned int>& indices);
	virtual ~IndexBuffer() = default;
	virtual void Bind(Graphics* pGfx) noexcept override;
	UINT GetCount() const noexcept { return count; }

private:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

