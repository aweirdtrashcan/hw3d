#pragma once

#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics* pGfx, D3D11_PRIMITIVE_TOPOLOGY topology);
	virtual void Bind(Graphics* pGfx) noexcept override;
	static std::shared_ptr<Bindable> Resolve(Graphics* gfx, D3D11_PRIMITIVE_TOPOLOGY topology);
	static std::string GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology);

private:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};
