#pragma once

#include "TestObject.h"
#include "ShaderStructs.h"

class Suzanne : public TestObject
{
public:
	Suzanne(Graphics* gfx,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist);

private:
	PSMaterialConstant materialConstants;
};