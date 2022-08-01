#include "Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Game::Game(const float circleInnerRadius /*= 20.0f*/,
	const float circleRadius /*= 60.0f*/,
	const float circleOuterRadius /*= 70.0f*/,
	const int circleResolution /*= 72*/,
	glm::vec4 color_background /*= glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)*/,
	glm::vec4 color_shrink_circle /*= glm::vec4(0.8f, 0.5f, 1.0f, 1.0f)*/,
	glm::vec4 color_static_circle /*= glm::vec4(0.8f, 0.5f, 1.0f, 1.0f)*/,
	glm::vec4 color_center /*= glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)*/,
	const float circle_init_size /*= 4.5f*/,
	const float circle_shrink_speed /*= 0.01f*/)
	: CIRCLE_INNER_RADIUS(circleInnerRadius),
	CIRCLE_RADIUS(circleRadius), 
	CIRCLE_OUTER_RADIUS(circleOuterRadius),
	CIRCLE_RESOLUTION(circleResolution), 
	orthoMatrix(glm::mat4(1.0f)),
	COLOR_BACKGROUND(color_background), 
	COLOR_SHRINK_CIRCLE(color_shrink_circle), 
	COLOR_STATIC_CIRCLE(color_static_circle),
	COLOR_CENTER(color_center), 
	CIRCLE_INIT_SIZE(circle_init_size), 
	CIRCLE_SHRINK_SPEED(circle_shrink_speed)

{
	sound_engine = irrklang::createIrrKlangDevice();
	stbi_set_flip_vertically_on_load(true);
	orthoMatrix = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
}

Game::~Game()
{
	sound_engine->drop();
}

void Game::CreateBasicCircle(const glm::vec3 center, const int index)
{
	BasicCircle* basic = new BasicCircle;
	basic->shrinkCircleData = CreateShrinkCircleData(center);
	basic->staticCircleData = CreateStaticCircleData(center);
	basic->textureCircleData = CreateTextureCircleData(center, index);
	basic->type = ENTITY_TYPE::BASIC;
	entity_buffer.push_back(basic);
}


ShrinkCircleData* Game::CreateShrinkCircleData(const glm::vec3 center)
{
	ShrinkCircleData* shrinkCircle = new ShrinkCircleData(CIRCLE_INIT_SIZE, CIRCLE_SHRINK_SPEED);
	ASSERT(glCreateVertexArrays(1, &shrinkCircle->vao)); // maybe
	ASSERT(glBindVertexArray(shrinkCircle->vao));

	ASSERT(glGenBuffers(1, &shrinkCircle->vbo));
	ASSERT(glBindBuffer(GL_ARRAY_BUFFER, shrinkCircle->vbo));

	shrinkCircle->center = center;
	GenCircleData(shrinkCircle->points, 
		shrinkCircle->indices, 
		shrinkCircle->center,
		CIRCLE_RADIUS - 6.0f, // hard coded values
		CIRCLE_RADIUS, 
		CIRCLE_RADIUS + 3.0f, // hard coded values
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND, 
		COLOR_SHRINK_CIRCLE, 
		COLOR_CENTER);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, shrinkCircle->points.size() * sizeof(float), &(shrinkCircle->points[0]), GL_STATIC_DRAW));

	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &(shrinkCircle->ebo)));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shrinkCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, shrinkCircle->indices.size() * sizeof(unsigned int), &(shrinkCircle->indices[0]), GL_STATIC_DRAW));

	shrinkCircle->shader = Shader("src/shaders/shrinkVertex.shader", "src/shaders/shrinkFragment.shader");
	shrinkCircle->shader.useProgram();

	ASSERT(int uniformLocation = glGetUniformLocation(shrinkCircle->shader.getProgramID(), "orthoMatrix"));
	ASSERT(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix)));

	shrinkCircle->shrinkMatrix = glm::scale(glm::vec3(shrinkCircle->shrinkFactor, shrinkCircle->shrinkFactor, shrinkCircle->shrinkFactor));
	shrinkCircle->toOrigin = glm::translate(glm::vec3(-1.0f) * shrinkCircle->center);
	shrinkCircle->fromOrigin = glm::translate(shrinkCircle->center);

	shrinkCircle->shrinkMatrixLoc = glGetUniformLocation(shrinkCircle->shader.getProgramID(), "shrinkMatrix");
	glUniformMatrix4fv(shrinkCircle->shrinkMatrixLoc, 1, GL_FALSE, glm::value_ptr(shrinkCircle->shrinkMatrix));

	uniformLocation = glGetUniformLocation(shrinkCircle->shader.getProgramID(), "toOrigin");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(shrinkCircle->toOrigin));

	uniformLocation = glGetUniformLocation(shrinkCircle->shader.getProgramID(), "fromOrigin");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(shrinkCircle->fromOrigin));

	ASSERT(glBindVertexArray(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return shrinkCircle;

}

StaticCircleData* Game::CreateStaticCircleData(const glm::vec3 center)
{
	StaticCircleData* staticCircle = new StaticCircleData();
	glCreateVertexArrays(1, &staticCircle->vao);
	glBindVertexArray(staticCircle->vao);

	glGenBuffers(1, &staticCircle->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, staticCircle->vbo);
	
	staticCircle->center = center;
	GenCircleData(staticCircle->points, 
		staticCircle->indices, 
		staticCircle->center,
		CIRCLE_INNER_RADIUS, 
		CIRCLE_RADIUS, 
		CIRCLE_OUTER_RADIUS, 
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND, 
		COLOR_STATIC_CIRCLE, 
		COLOR_CENTER);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, staticCircle->points.size() * sizeof(float), &(staticCircle->points[0]), GL_STATIC_DRAW));

	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &(staticCircle->ebo)));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, staticCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, staticCircle->indices.size() * sizeof(unsigned int), &(staticCircle->indices[0]), GL_STATIC_DRAW));

	staticCircle->shader = Shader("src/shaders/staticVertex.shader", "src/shaders/staticFragment.shader");
	staticCircle->shader.useProgram();

	int uniformLocation = glGetUniformLocation(staticCircle->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));
	


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return staticCircle;
}

TextureCircleData* Game::CreateTextureCircleData(const glm::vec3 center, const int index)
{
	TextureCircleData* textureCircle = new TextureCircleData();

	ASSERT(glCreateVertexArrays(1, &textureCircle->vao));
	ASSERT(glBindVertexArray(textureCircle->vao));

	ASSERT(glGenBuffers(1, &textureCircle->vbo));
	ASSERT(glBindBuffer(GL_ARRAY_BUFFER, textureCircle->vbo));

	textureCircle->index = index;
	textureCircle->center = center;
	GenTextureData(textureCircle->points, textureCircle->indices, textureCircle->center /* default texture dimensions */);
	
	ASSERT(glBufferData(GL_ARRAY_BUFFER, textureCircle->points.size() * sizeof(float), &textureCircle->points[0], GL_STATIC_DRAW));
	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &textureCircle->ebo));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, textureCircle->indices.size() * sizeof(unsigned int), &textureCircle->indices[0], GL_STATIC_DRAW));

	textureCircle->shader = Shader("src/shaders/textureVertex.shader", "src/shaders/textureFragment.shader");
	textureCircle->shader.useProgram();

	int uniformLocation = glGetUniformLocation(textureCircle->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	std::string texturePath;
	switch (index)
	{
		case 1:
			texturePath = "res/textures/numbers/1.png";
			break;
		case 2:
			texturePath = "res/textures/numbers/2.png";
			break;
		case 3:
			texturePath = "res/textures/numbers/3.png";
			break;
		case 4:
			texturePath = "res/textures/numbers/4.png";
			break;
	}

	int x, y, n;
	unsigned char* imageData = stbi_load(texturePath.c_str(), &x, &y, &n, 0);

	if (!imageData)
	{
		std::cout << "Failed to load image data!" << std::endl;
		__debugbreak();
	}

	ASSERT(glGenTextures(1, &textureCircle->textureID));
	ASSERT(glBindTexture(GL_TEXTURE_2D, textureCircle->textureID));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));

	stbi_image_free(imageData);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureCircle;
}

TextureScoreData* Game::CreateTextureScoreData(const glm::vec3 center, SCORE score)
{
	TextureScoreData* textureScore = new TextureScoreData;

	glCreateVertexArrays(1, &textureScore->vao);
	glBindVertexArray(textureScore->vao);

	glGenBuffers(1, &textureScore->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, textureScore->vbo);

	textureScore->center = center;
	GenTextureData(textureScore->points, textureScore->indices, center, 20.0f, 10.0f);

	glBufferData(GL_ARRAY_BUFFER, textureScore->points.size() * sizeof(float), &textureScore->points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &textureScore->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureScore->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, textureScore->indices.size() * sizeof(unsigned int), &textureScore->indices[0], GL_STATIC_DRAW);

	textureScore->shader = Shader("src/shaders/textureScoreVertex.shader", "src/shaders/textureScoreFragment.shader");
	textureScore->shader.useProgram();

	int uniformLocation = glGetUniformLocation(textureScore->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	textureScore->colorUniformLoc = glGetUniformLocation(textureScore->shader.getProgramID(), "color");
	glUniform4f(textureScore->colorUniformLoc, 1.0f, 1.0f, 1.0f, textureScore->alpha);

	std::string texturePath;
	switch (score)
	{
	case SCORE::FAIL:
		texturePath = "res/textures/scores/x.png";
		break;
	case SCORE::FIFTY:
		texturePath = "res/textures/scores/50.png";
		break;
	case SCORE::HUNDRED:
		texturePath = "res/textures/scores/100.png";
		break;
	}

	int x, y, n;
	unsigned char* imageData = stbi_load(texturePath.c_str(), &x, &y, &n, 0);

	if (!imageData)
	{
		std::cout << "Failed to load image data!" << std::endl;
		__debugbreak();
	}

	glGenTextures(1, &textureScore->textureID);
	glBindTexture(GL_TEXTURE_2D, textureScore->textureID);
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));

	stbi_image_free(imageData);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureScore;
}

SliderData* Game::CreateSliderData(const glm::vec3 startPos, const glm::vec3 endPos, const bool repeat)
{
	SliderData* sliderData = new SliderData;
	glCreateVertexArrays(1, &sliderData->vao);
	glBindVertexArray(sliderData->vao);

	glGenBuffers(1, &sliderData->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, sliderData->vbo);

	GenSliderData(sliderData->points,
		sliderData->indices,
		startPos,
		endPos,
		CIRCLE_INNER_RADIUS,
		CIRCLE_RADIUS,
		CIRCLE_OUTER_RADIUS,
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND,
		COLOR_STATIC_CIRCLE,
		COLOR_CENTER);

	glBufferData(GL_ARRAY_BUFFER, sliderData->points.size() * sizeof(float), &sliderData->points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &sliderData->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sliderData->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sliderData->indices.size() * sizeof(unsigned int), &sliderData->indices[0], GL_STATIC_DRAW);

	sliderData->shader = Shader("src/shaders/staticVertex.shader", "src/shaders/staticFragment.shader");
	sliderData->shader.useProgram();

	int uniformLocation = glGetUniformLocation(sliderData->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return sliderData;
}

void Game::Draw()
{
	for (int i = entity_buffer.size() - 1; i >= 0; i--)
	{
		if (entity_buffer[i]->type == ENTITY_TYPE::BASIC)
			BasicCircleDraw((BasicCircle*)entity_buffer[i]);
		else if (entity_buffer[i]->type == ENTITY_TYPE::SLIDER)
		{
			// SliderDraw();
		}
		else
		{
			std::cout << "Unknown Entity" << std::endl;
			__debugbreak();
		}
	}

	for (int i = 0; i < score_entity_buffer.size(); i++)
	{
		TextureScoreDraw(score_entity_buffer[i]);
	}
}

void Game::BasicCircleDraw(BasicCircle* circle)
{
	// shrink circle
	glBindVertexArray(circle->shrinkCircleData->vao);
	circle->shrinkCircleData->shader.useProgram();

	if (circle->shrinkCircleData->shrinkFactor > 1)
	{
		circle->shrinkCircleData->shrinkFactor -= circle->shrinkCircleData->shrinkSpeed;
		float scale = circle->shrinkCircleData->shrinkFactor;
		circle->shrinkCircleData->shrinkMatrix = glm::scale(glm::vec3(scale, scale, scale));
		glUniformMatrix4fv(circle->shrinkCircleData->shrinkMatrixLoc, 1, GL_FALSE, glm::value_ptr(circle->shrinkCircleData->shrinkMatrix));
	}
	else if (circle->shrinkCircleData->shrinkFactor > 0.75)
	{
		circle->shrinkCircleData->shrinkFactor -= circle->shrinkCircleData->shrinkSpeed;
	}
	else
	{
		OnEvent(GLFW_KEY_Z, GLFW_PRESS, -1.0f, -1.0f);
		return;
	}

	ASSERT(glDrawElements(GL_TRIANGLES, circle->shrinkCircleData->indices.size(), GL_UNSIGNED_INT, (void*)0));

	// static circle
	glBindVertexArray(circle->staticCircleData->vao);
	circle->staticCircleData->shader.useProgram();
	glDrawElements(GL_TRIANGLES, circle->staticCircleData->indices.size(), GL_UNSIGNED_INT, (void*)0);

	// texture
	glBindVertexArray(circle->textureCircleData->vao);
	circle->textureCircleData->shader.useProgram();
	glBindTexture(GL_TEXTURE_2D, circle->textureCircleData->textureID);
	glDrawElements(GL_TRIANGLES, circle->textureCircleData->indices.size(), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::TextureScoreDraw(TextureScoreData* score)
{
	glBindVertexArray(score->vao);
	score->shader.useProgram();
	glBindTexture(GL_TEXTURE_2D, score->textureID);

	if (score->alpha > 0)
	{
		score->alpha -= 0.01f;
		glUniform4f(score->colorUniformLoc, 1.0f, 1.0f, 1.0f, score->alpha);
		glDrawElements(GL_TRIANGLES, score->indices.size(), GL_UNSIGNED_INT, (void*)0);
	}
	else
	{
		delete score;
		score_entity_buffer.pop_front();
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::SliderDraw(Slider* slider)
{
	if (slider->basicCircle == nullptr)
	{
		std::cout << "Slider basic circle was deleted!" << std::endl;
		__debugbreak();
	}
	else
		BasicCircleDraw(slider->basicCircle);

	glBindVertexArray(slider->sliderData->vao);
	slider->sliderData->shader.useProgram();
	glDrawElements(GL_TRIANGLES, slider->sliderData->indices.size(), GL_UNSIGNED_INT, (void*)0);

}

void Game::OnEvent(int key, int action, double x, double y)
{
	if (entity_buffer.size() == 0)
	{
		return;
	}
	else if (key != GLFW_KEY_Z && key != GLFW_KEY_X && key != GLFW_MOUSE_BUTTON_1)
		return;
	else
	{
		switch (entity_buffer[0]->type)
		{
		case ENTITY_TYPE::BASIC:

			if (action == GLFW_RELEASE)
				return;

			BasicCircle* circle = (BasicCircle*)entity_buffer[0];

			bool in_range = x >= circle->shrinkCircleData->center.x - 400.0f && x <= circle->shrinkCircleData->center.x + 400.0f &&
				y >= circle->shrinkCircleData->center.y - 400.0f && y <= circle->shrinkCircleData->center.y + 400.0f;

			bool in_circle = y <= sqrt(CIRCLE_RADIUS * CIRCLE_RADIUS - pow(x - circle->shrinkCircleData->center.x, 2)) + circle->shrinkCircleData->center.y &&
				y >= circle->shrinkCircleData->center.y - sqrt(CIRCLE_RADIUS * CIRCLE_RADIUS - pow(x - circle->shrinkCircleData->center.x, 2));

			if (x == -1) // circle expires
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::FAIL));
			}
			else if (!in_range) // if out of bounds, ignore input
				return;
			else if (!in_circle) // for follow, scores in circles aren't necessary. Push entity score buffer
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::FAIL));
			}
			else if (circle->shrinkCircleData->shrinkFactor < 1.2f && circle->shrinkCircleData->shrinkFactor > 1.0f)
			{
				// success
			}

			else if (circle->shrinkCircleData->shrinkFactor < 1.4f && circle->shrinkCircleData->shrinkFactor > 1.2f)
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::HUNDRED));
			}
			else if (circle->shrinkCircleData->shrinkFactor < 1.0f && circle->shrinkCircleData->shrinkFactor > 0.8f)
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::HUNDRED));
			}

			else if (circle->shrinkCircleData->shrinkFactor < 1.6 && circle->shrinkCircleData->shrinkFactor > 1.4f)
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::FIFTY));
			}
			else if (circle->shrinkCircleData->shrinkFactor < 0.8f && circle->shrinkCircleData->shrinkFactor > 0.6f)
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::FIFTY));
			}

			else
			{
				score_entity_buffer.push_back(CreateTextureScoreData(circle->shrinkCircleData->center, SCORE::FAIL));
			}
			delete circle->shrinkCircleData;
			delete circle->staticCircleData;
			delete circle->textureCircleData;
			delete entity_buffer[0];
			entity_buffer.pop_front();
			break;
		}
	}
}

void Game::GenCircleData(std::vector<float>& points, 
	std::vector<unsigned int>& indices, 
	const glm::vec3 center,
	const float innerRadius, 
	const float middleRadius, 
	const float outerRadius, 
	const int resolution,
	const glm::vec4 backgroundColor, 
	const glm::vec4 circleColor, 
	const glm::vec4 circleCenterColor)
{
	points.push_back(center.x);
	points.push_back(center.y);
	points.push_back(center.z);
	points.push_back(circleCenterColor.x);
	points.push_back(circleCenterColor.y);
	points.push_back(circleCenterColor.z);
	points.push_back(circleCenterColor.w);

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
}

void Game::GenTextureData(std::vector<float>& points, 
	std::vector<unsigned int>& indices, 
	const glm::vec3 center, 
	const float width, 
	const float height)
{
	// bottom left
	points.push_back(-width + center.x);
	points.push_back(-height + center.y);
	points.push_back(0.0f); // z coordinate
	points.push_back(0.0f); // texture coordinates
	points.push_back(0.0f);

	// bottom right
	points.push_back(width + center.x);
	points.push_back(-height + center.y);
	points.push_back(0.03f); // z coordinate
	points.push_back(1.0f); // texture coordinates
	points.push_back(0.0f);

	// top left
	points.push_back(-width + center.x);
	points.push_back(height + center.y);
	points.push_back(0.0f); // z coordinate
	points.push_back(0.0f); // texture coordinates
	points.push_back(1.0f);

	// top right
	points.push_back(width + center.x);
	points.push_back(height + center.y);
	points.push_back(0.0f); // z coordinate
	points.push_back(1.0f); // texture coordinates
	points.push_back(1.0f);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(1);
}

void Game::GenSliderData(std::vector<float>& points, 
	std::vector<unsigned int>& indices, 
	const glm::vec3 startPos, 
	const glm::vec3 endPos, 
	const float innerRadius,
	const float middleRadius,
	const float outerRadius,
	const int resolution,
	const glm::vec4 backgroundColor,
	const glm::vec4 circleColor,
	const glm::vec4 circleCenterColor)
{
	// slider angle from +x axis for half circles
	bool endIsLeftOfStart = endPos.x < startPos.x;

	double x = abs(startPos.x - endPos.x);
	double y = abs(startPos.y - endPos.y);
	double angle = atan(y / x);

	if (endIsLeftOfStart)
	{
		angle = M_PI - angle;
	}

	// test
	// add second half circle, make i global

	points.push_back(startPos.x);
	points.push_back(startPos.y);
	points.push_back(startPos.z);
	points.push_back(circleCenterColor.x);
	points.push_back(circleCenterColor.y);
	points.push_back(circleCenterColor.z);
	points.push_back(circleCenterColor.w);

	for (int i = 0; i < resolution; i++)
	{
		// inner points
		float x = innerRadius * cos(i * M_PI / resolution + angle) + startPos.x;
		float y = innerRadius * sin(i * M_PI / resolution + angle) + startPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);

		// middle points
		x = innerRadius * cos(i * M_PI / resolution + angle) + startPos.x;
		y = innerRadius * sin(i * M_PI / resolution + angle) + startPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(circleColor.x);
		points.push_back(circleColor.y);
		points.push_back(circleColor.z);
		points.push_back(circleColor.w);

		// outer points
		x = innerRadius * cos(i * M_PI / resolution + angle) + startPos.x;
		y = innerRadius * sin(i * M_PI / resolution + angle) + startPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);

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

		if (i == resolution - 1)
		{
			i++;
			// inner points
			float x = innerRadius * cos(i * M_PI / resolution + angle) + startPos.x;
			float y = innerRadius * sin(i * M_PI / resolution + angle) + startPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(backgroundColor.x);
			points.push_back(backgroundColor.y);
			points.push_back(backgroundColor.z);
			points.push_back(backgroundColor.w);

			// middle points
			x = innerRadius * cos(i * M_PI / resolution + angle) + startPos.x;
			y = innerRadius * sin(i * M_PI / resolution + angle) + startPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(circleColor.x);
			points.push_back(circleColor.y);
			points.push_back(circleColor.z);
			points.push_back(circleColor.w);

			// outer points
			x = innerRadius * cos(i * M_PI / resolution + angle) + startPos.x;
			y = innerRadius * sin(i * M_PI / resolution + angle) + startPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(backgroundColor.x);
			points.push_back(backgroundColor.y);
			points.push_back(backgroundColor.z);
			points.push_back(backgroundColor.w);
		}
	}
}

void Game::KeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	y = SCREEN_HEIGHT - y;
	game->OnEvent(key, action, x, y);
}

void Game::MouseInput(GLFWwindow* window, int button, int action, int mods)
{
	Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	y = SCREEN_HEIGHT - y;
	game->OnEvent(button, action, x, y);
}
