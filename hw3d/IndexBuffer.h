#pragma once

#include "Bindable.h"

#include <d3d11.h>
#include <wrl.h>
#include <vector>

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics* pGfx, const std::vector<unsigned int>& indices, const std::string& tag);
	virtual ~IndexBuffer() = default;
	virtual void Bind(Graphics* pGfx) noexcept override;
	UINT GetCount() const noexcept { return count; }
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, const std::vector<unsigned int>& indices, const std::string& tag);
	static std::string GenerateUID(const std::vector<unsigned int>& indices, const std::string& tag);

private:
	UINT count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

