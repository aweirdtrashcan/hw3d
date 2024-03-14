#pragma once

#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(Graphics* pGfx, D3D11_PRIMITIVE_TOPOLOGY topology);
	virtual void Bind(Graphics* pGfx) noexcept override;

private:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};
