#include "osuCircleStatic.h"

#define M_PI 3.14159265358979323846

void osuCircleStatic(std::vector<float>& points, std::vector<unsigned int>& indices,
	const glm::vec3 center, const float innerRadius, const float middleRadius, const float outerRadius, const int resolution, 
	const glm::vec4 backgroundColor, const glm::vec4 circleColor, const glm::vec4 circleCenterColor)
{
	points.push_back(center.x);
	points.push_back(center.y);
	points.push_back(center.z);
	points.push_back(circleCenterColor.x);
	points.push_back(circleCenterColor.y);
	points.push_back(circleCenterColor.z);
	points.push_back(circleCenterColor.w);
	points.push_back(-1.0f); // texture coordinates
	points.push_back(-1.0f);


	for (int i = 0; i < resolution; i++)
	{
		// inner points
		float x = innerRadius * cos(i * M_PI / (resolution / 2)) + center.x;
		float y = innerRadius * sin(i * M_PI / (resolution / 2)) + center.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);
		points.push_back(-1.0f); // texture coordinates
		points.push_back(-1.0f);

		// middle points
		x = middleRadius * cos(i * M_PI / (resolution / 2)) + center.x;
		y = middleRadius * sin(i * M_PI / (resolution / 2)) + center.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(circleColor.x);
		points.push_back(circleColor.y);
		points.push_back(circleColor.z);
		points.push_back(circleColor.w);
		points.push_back(-1.0f); // texture coordinates
		points.push_back(-1.0f);

		// outer points
		x = outerRadius * cos(i * M_PI / (resolution / 2)) + center.x;
		y = outerRadius * sin(i * M_PI / (resolution / 2)) + center.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);
		points.push_back(-1.0f); // texture coordinates
		points.push_back(-1.0f);

		if (i == resolution - 1)
		{
			// center triangle (0 1 4)
			indices.push_back(0);
			indices.push_back(i * 3 + 1);
			indices.push_back(1);

			// inner triangle 1 (1 2 4)		
			indices.push_back(i * 3 + 1);
			indices.push_back(i * 3 + 2);
			indices.push_back(1);

			// inner triangle 2 (2 4 5)
			indices.push_back(i * 3 + 2);
			indices.push_back(1);
			indices.push_back(2);

			// outer triangle 1 (2 3 5)
			indices.push_back(i * 3 + 2);
			indices.push_back(i * 3 + 3);
			indices.push_back(2);

			// outer triangle 2 (3 5 6)
			indices.push_back(i * 3 + 3);
			indices.push_back(2);
			indices.push_back(3);
		}

		else
		{
			// center triangle (0 1 4)
			indices.push_back(0);
			indices.push_back(i * 3 + 1);
			indices.push_back(i * 3 + 4);

			// inner triangle 1 (1 2 4)		
			indices.push_back(i * 3 + 1);
			indices.push_back(i * 3 + 2);
			indices.push_back(i * 3 + 4);

			// inner triangle 2 (2 4 5)
			indices.push_back(i * 3 + 2);
			indices.push_back(i * 3 + 4);
			indices.push_back(i * 3 + 5);

			// outer triangle 1 (2 3 5)
			indices.push_back(i * 3 + 2);
			indices.push_back(i * 3 + 3);
			indices.push_back(i * 3 + 5);

			// outer triangle 2 (3 5 6)
			indices.push_back(i * 3 + 3);
			indices.push_back(i * 3 + 5);
			indices.push_back(i * 3 + 6);
		}
	}

	// bottom left
	points.push_back(-20.0f + center.x);
	points.push_back(-20.0f + center.y);
	points.push_back(0.0f); // z coordinate
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f); // texture coordinates
	points.push_back(0.0f);

	// bottom right
	points.push_back(20.0f + center.x);
	points.push_back(-20.0f + center.y);
	points.push_back(0.03f); // z coordinate
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(1.0f); // texture coordinates
	points.push_back(0.0f);

	// top left
	points.push_back(-20.0f + center.x);
	points.push_back(30.0f + center.y);
	points.push_back(0.0f); // z coordinate
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f); // texture coordinates
	points.push_back(1.0f);

	// top right
	points.push_back(20.0f + center.x);
	points.push_back(30.0f + center.y);
	points.push_back(0.0f); // z coordinate
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(0.0f);
	points.push_back(1.0f); // texture coordinates
	points.push_back(1.0f);

	indices.push_back(points.size() / 9 - 4);
	indices.push_back(points.size() / 9 - 3);
	indices.push_back(points.size() / 9 - 2);

	indices.push_back(points.size() / 9 - 3);
	indices.push_back(points.size() / 9 - 2);
	indices.push_back(points.size() / 9 - 1);

}