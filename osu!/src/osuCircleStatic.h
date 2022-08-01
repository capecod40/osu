#pragma once

#include <vector>
#include "glm/glm.hpp"

void osuCircleStatic(std::vector<float>& points, std::vector<unsigned int>& indices,
	const glm::vec3 center, const float innerRadius, const float middleRadius, const float outerRadius, const int resolution,
	const glm::vec4 backgroundColor, const glm::vec4 circleColor, const glm::vec4 circleCenterColor);