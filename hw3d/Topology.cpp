#include "Topology.h"

Topology::Topology(Graphics* pGfx, D3D11_PRIMITIVE_TOPOLOGY topology)
	:
	topology(topology)
{
}

void Topology::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->IASetPrimitiveTopology(topology);
}
