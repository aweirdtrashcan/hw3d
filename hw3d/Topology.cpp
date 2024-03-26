#include "Topology.h"

#include "BindableCodex.h"

Topology::Topology(Graphics* pGfx, D3D11_PRIMITIVE_TOPOLOGY topology)
	:
	topology(topology)
{
}

void Topology::Bind(Graphics* pGfx) noexcept
{
	GetContext(pGfx)->IASetPrimitiveTopology(topology);
}

std::shared_ptr<Bindable> Topology::Resolve(Graphics* gfx, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	return Codex::Resolve<Topology>(gfx, topology);
}

std::string Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	return std::format("{}#{}", typeid(Topology).name(), (int)topology);
}
