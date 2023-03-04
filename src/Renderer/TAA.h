#pragma once
#include"glm/glm.hpp"
#include<vector>

inline float CreateHaltonSequence(unsigned int index, int base)
{
	float f = 1;
	float r = 0;
	int current = index;
	do
	{
		f = f / base;
		r = r + f * (current % base);
		current = (int)glm::floor(current / base);
	} while (current > 0);

	return r;
}


struct TaaData
{

	std::vector<glm::vec4>haltonSequence;
	unsigned int haltonUbo;
	glm::mat4 prevView;
	glm::mat4 prevProj;

	TaaData()
	{
		for (int iter = 0; iter < 16; iter++)
		{
			haltonSequence.push_back(glm::vec4(CreateHaltonSequence(iter + 1, 2), CreateHaltonSequence(iter + 1, 3),0.0f,0.0f));
		}
	}
};

