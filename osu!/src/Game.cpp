#include "Game.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Game::Game(GLFWwindow* win, 
	const double bpm, 
	const int beatsPerBar, 
	const float circleInnerRadius /*= 20.0f*/,
	const float circleRadius /*= 60.0f*/,
	const float circleOuterRadius /*= 70.0f*/,
	const int circleResolution /*= 72*/,
	glm::vec4 color_background /*= glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)*/,
	glm::vec4 color_shrink_circle /*= glm::vec4(0.8f, 0.5f, 1.0f, 1.0f)*/,
	glm::vec4 color_static_circle /*= glm::vec4(0.8f, 0.5f, 1.0f, 1.0f)*/,
	glm::vec4 color_center /*= glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)*/,
	const float circle_init_size /*= 4.5f*/,
	const float circle_shrink_speed /*= 0.01f*/, 
	const float slider_speed /*= 1.0f*/, 
	const int hold /*= false*/, 
	const bool menu /*= false*/)
	: window(win),
	BPM(bpm), 
	BEATS_PER_BAR(beatsPerBar), 
	CIRCLE_INNER_RADIUS(circleInnerRadius),
	CIRCLE_RADIUS(circleRadius), 
	CIRCLE_OUTER_RADIUS(circleOuterRadius),
	CIRCLE_RESOLUTION(circleResolution),
	orthoMatrix(glm::mat4(1.0f)),
	COLOR_BACKGROUND(color_background), 
	COLOR_SHRINK_CIRCLE(color_shrink_circle), 
	COLOR_STATIC_CIRCLE(color_static_circle),
	COLOR_CENTER(color_center), 
	CIRCLE_INIT_SIZE(circle_init_size), 
	CIRCLE_SHRINK_SPEED(circle_shrink_speed), 
	SLIDER_SPEED(slider_speed), 
	keyHold(hold), 
	inMenu(menu)
{
	sound_engine = irrklang::createIrrKlangDevice();
	stbi_set_flip_vertically_on_load(true);
	orthoMatrix = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
	this->menu = CreateDataMenu();
	this->menu_logo = CreateDataTextureCircle(glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT * 0.88f, 0.0f), -1);
}

Game::~Game()
{
	sound_engine->drop();
}

BasicCircle* Game::CreateBasicCircle(const glm::vec3 center, const int index)
{
	BasicCircle* basic = new BasicCircle;
	basic->dataShrinkCircle = CreateDataShrinkCircle(center);
	basic->dataStaticCircle = CreateDataStaticCircle(center);
	basic->dataTextureCircle = CreateDataTextureCircle(center, index);
	basic->type = ENTITY_TYPE::BASIC;
	entity_buffer.push_back(basic);
	return basic; // needed for basic circle in slider
}


DataShrinkCircle* Game::CreateDataShrinkCircle(const glm::vec3 center)
{
	DataShrinkCircle* dataShrinkCircle = new DataShrinkCircle(CIRCLE_INIT_SIZE, CIRCLE_SHRINK_SPEED);
	ASSERT(glCreateVertexArrays(1, &dataShrinkCircle->vao)); // maybe
	ASSERT(glBindVertexArray(dataShrinkCircle->vao));

	ASSERT(glGenBuffers(1, &dataShrinkCircle->vbo));
	ASSERT(glBindBuffer(GL_ARRAY_BUFFER, dataShrinkCircle->vbo));

	dataShrinkCircle->center = center;
	GenDataCircle(dataShrinkCircle->points, 
		dataShrinkCircle->indices, 
		dataShrinkCircle->center,
		CIRCLE_RADIUS - 6.0f, // hard coded values
		CIRCLE_RADIUS, 
		CIRCLE_RADIUS + 3.0f, // hard coded values
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND, 
		COLOR_SHRINK_CIRCLE, 
		COLOR_CENTER);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, dataShrinkCircle->points.size() * sizeof(float), &(dataShrinkCircle->points[0]), GL_STATIC_DRAW));

	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &(dataShrinkCircle->ebo)));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataShrinkCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataShrinkCircle->indices.size() * sizeof(unsigned int), &(dataShrinkCircle->indices[0]), GL_STATIC_DRAW));

	dataShrinkCircle->shader = Shader("src/shaders/shrinkVertex.shader", "src/shaders/shrinkFragment.shader");
	dataShrinkCircle->shader.useProgram();

	ASSERT(int uniformLocation = glGetUniformLocation(dataShrinkCircle->shader.getProgramID(), "orthoMatrix"));
	ASSERT(glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix)));

	dataShrinkCircle->shrinkMatrix = glm::scale(glm::vec3(dataShrinkCircle->shrinkFactor, dataShrinkCircle->shrinkFactor, dataShrinkCircle->shrinkFactor));
	dataShrinkCircle->toOrigin = glm::translate(glm::vec3(-1.0f) * dataShrinkCircle->center);
	dataShrinkCircle->fromOrigin = glm::translate(dataShrinkCircle->center);

	dataShrinkCircle->shrinkMatrixLoc = glGetUniformLocation(dataShrinkCircle->shader.getProgramID(), "shrinkMatrix");
	glUniformMatrix4fv(dataShrinkCircle->shrinkMatrixLoc, 1, GL_FALSE, glm::value_ptr(dataShrinkCircle->shrinkMatrix));

	uniformLocation = glGetUniformLocation(dataShrinkCircle->shader.getProgramID(), "toOrigin");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(dataShrinkCircle->toOrigin));

	uniformLocation = glGetUniformLocation(dataShrinkCircle->shader.getProgramID(), "fromOrigin");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(dataShrinkCircle->fromOrigin));

	ASSERT(glBindVertexArray(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return dataShrinkCircle;

}

DataStaticCircle* Game::CreateDataStaticCircle(const glm::vec3 center)
{
	DataStaticCircle* dataStaticCircle = new DataStaticCircle();
	glCreateVertexArrays(1, &dataStaticCircle->vao);
	glBindVertexArray(dataStaticCircle->vao);

	glGenBuffers(1, &dataStaticCircle->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dataStaticCircle->vbo);
	
	dataStaticCircle->center = center;
	GenDataCircle(dataStaticCircle->points, 
		dataStaticCircle->indices, 
		dataStaticCircle->center,
		CIRCLE_INNER_RADIUS, 
		CIRCLE_RADIUS, 
		CIRCLE_OUTER_RADIUS, 
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND, 
		COLOR_STATIC_CIRCLE, 
		COLOR_CENTER);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, dataStaticCircle->points.size() * sizeof(float), &(dataStaticCircle->points[0]), GL_STATIC_DRAW));

	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &(dataStaticCircle->ebo)));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataStaticCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataStaticCircle->indices.size() * sizeof(unsigned int), &(dataStaticCircle->indices[0]), GL_STATIC_DRAW));

	dataStaticCircle->shader = Shader("src/shaders/staticVertex.shader", "src/shaders/staticFragment.shader");
	dataStaticCircle->shader.useProgram();

	int uniformLocation = glGetUniformLocation(dataStaticCircle->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));
	


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	return dataStaticCircle;
}

DataTextureCircle* Game::CreateDataTextureCircle(const glm::vec3 center, const int index)
{
	DataTextureCircle* dataTextureCircle = new DataTextureCircle();

	ASSERT(glCreateVertexArrays(1, &dataTextureCircle->vao));
	ASSERT(glBindVertexArray(dataTextureCircle->vao));

	ASSERT(glGenBuffers(1, &dataTextureCircle->vbo));
	ASSERT(glBindBuffer(GL_ARRAY_BUFFER, dataTextureCircle->vbo));

	dataTextureCircle->index = index;
	dataTextureCircle->center = center;

	if (index == -1)
		GenDataTexture(dataTextureCircle->points, dataTextureCircle->indices, dataTextureCircle->center, SCREEN_HEIGHT * 0.10f, SCREEN_HEIGHT * 0.10f);
	else
		GenDataTexture(dataTextureCircle->points, dataTextureCircle->indices, dataTextureCircle->center /* default texture dimensions */);
	
	ASSERT(glBufferData(GL_ARRAY_BUFFER, dataTextureCircle->points.size() * sizeof(float), &dataTextureCircle->points[0], GL_STATIC_DRAW));
	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &dataTextureCircle->ebo));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataTextureCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataTextureCircle->indices.size() * sizeof(unsigned int), &dataTextureCircle->indices[0], GL_STATIC_DRAW));

	dataTextureCircle->shader = Shader("src/shaders/textureVertex.shader", "src/shaders/textureFragment.shader");
	dataTextureCircle->shader.useProgram();

	int uniformLocation = glGetUniformLocation(dataTextureCircle->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	std::string texturePath;
	switch (index)
	{
		case -1:
			texturePath = "res/textures/credits/menu_logo.png";
			break;
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

	ASSERT(glGenTextures(1, &dataTextureCircle->textureID));
	ASSERT(glBindTexture(GL_TEXTURE_2D, dataTextureCircle->textureID));
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
	glUseProgram(0);
	return dataTextureCircle;
}

DataTextureScore* Game::CreateDataTextureScore(const glm::vec3 center, SCORE score)
{
	DataTextureScore* dataTextureScore = new DataTextureScore;

	glCreateVertexArrays(1, &dataTextureScore->vao);
	glBindVertexArray(dataTextureScore->vao);

	glGenBuffers(1, &dataTextureScore->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dataTextureScore->vbo);

	dataTextureScore->center = center;
	GenDataTexture(dataTextureScore->points, dataTextureScore->indices, center, 20.0f, 10.0f);

	glBufferData(GL_ARRAY_BUFFER, dataTextureScore->points.size() * sizeof(float), &dataTextureScore->points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &dataTextureScore->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataTextureScore->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataTextureScore->indices.size() * sizeof(unsigned int), &dataTextureScore->indices[0], GL_STATIC_DRAW);

	dataTextureScore->shader = Shader("src/shaders/textureScoreVertex.shader", "src/shaders/textureScoreFragment.shader");
	dataTextureScore->shader.useProgram();

	int uniformLocation = glGetUniformLocation(dataTextureScore->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	dataTextureScore->colorUniformLoc = glGetUniformLocation(dataTextureScore->shader.getProgramID(), "color");
	glUniform4f(dataTextureScore->colorUniformLoc, 1.0f, 1.0f, 1.0f, dataTextureScore->alpha);

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

	glGenTextures(1, &dataTextureScore->textureID);
	glBindTexture(GL_TEXTURE_2D, dataTextureScore->textureID);
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
	glUseProgram(0);
	return dataTextureScore;
}

Slider* Game::CreateSlider(const glm::vec3 startPos, const glm::vec3 endPos, const int index, const int repeat)
{
	Slider* slider = new Slider;
	slider->basicCircle = CreateBasicCircle(startPos, index); // pushes BasicCircle to buffer
	slider->dataSlider = CreateDataSlider(startPos, endPos, repeat);
	slider->dataSlidingCircle = CreateDataSlidingCircle(startPos, endPos, repeat);
	slider->dataClickSlidingCircle = CreateDataClickSlidingCircle(startPos, endPos, repeat);
	slider->type = ENTITY_TYPE::SLIDER;
	entity_buffer.push_back(slider);
	return slider; // not needed, kept for consistency with CreateBasicCircle
}

DataSlider* Game::CreateDataSlider(const glm::vec3 startPos, const glm::vec3 endPos, const int repeat)
{
	DataSlider* dataSlider = new DataSlider;
	glCreateVertexArrays(1, &dataSlider->vao);
	glBindVertexArray(dataSlider->vao);

	glGenBuffers(1, &dataSlider->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dataSlider->vbo);

	dataSlider->center = startPos;
	dataSlider->endPos = endPos;
	dataSlider->repeat = repeat;
	dataSlider->endIsLeftOfStart = dataSlider->endPos.x < dataSlider->center.x;
	dataSlider->endIsUnderStart = dataSlider->endPos.y < dataSlider->center.y;
	dataSlider->slope = (endPos.y - startPos.y) / (endPos.x - startPos.x);
	if (dataSlider->slope > 1.0f || dataSlider->slope < -1.0f)
		dataSlider->useYAxis = true;
	else
		dataSlider->useYAxis = false;

	GenDataSlider(dataSlider->points,
		dataSlider->indices,
		startPos,
		endPos,
		CIRCLE_INNER_RADIUS,
		CIRCLE_RADIUS,
		CIRCLE_OUTER_RADIUS,
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND,
		COLOR_STATIC_CIRCLE,
		COLOR_CENTER);

	glBufferData(GL_ARRAY_BUFFER, dataSlider->points.size() * sizeof(float), &dataSlider->points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &dataSlider->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataSlider->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSlider->indices.size() * sizeof(unsigned int), &dataSlider->indices[0], GL_STATIC_DRAW);

	dataSlider->shader = Shader("src/shaders/staticVertex.shader", "src/shaders/staticFragment.shader");
	dataSlider->shader.useProgram();

	int uniformLocation = glGetUniformLocation(dataSlider->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	return dataSlider;
}

DataSlidingCircle* Game::CreateDataSlidingCircle(const glm::vec3 startPos, const glm::vec3 endPos, const int repeat)
{
	DataSlidingCircle* dataSlidingCircle = new DataSlidingCircle;
	glCreateVertexArrays(1, &dataSlidingCircle->vao);
	glBindVertexArray(dataSlidingCircle->vao);

	glGenBuffers(1, &dataSlidingCircle->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dataSlidingCircle->vbo);

	dataSlidingCircle->center = startPos;
	GenDataCircle(dataSlidingCircle->points,
		dataSlidingCircle->indices,
		dataSlidingCircle->center,
		CIRCLE_INNER_RADIUS,
		CIRCLE_RADIUS,
		CIRCLE_OUTER_RADIUS,
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND,
		COLOR_STATIC_CIRCLE,
		COLOR_CENTER);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, dataSlidingCircle->points.size() * sizeof(float), &(dataSlidingCircle->points[0]), GL_STATIC_DRAW));

	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &(dataSlidingCircle->ebo)));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataSlidingCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSlidingCircle->indices.size() * sizeof(unsigned int), &(dataSlidingCircle->indices[0]), GL_STATIC_DRAW));

	dataSlidingCircle->shader = Shader("src/shaders/slidingBaseVertex.shader", "src/shaders/slidingBaseFragment.shader");
	dataSlidingCircle->shader.useProgram();

	int uniformLocation = glGetUniformLocation(dataSlidingCircle->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	dataSlidingCircle->translationMatrix = glm::translate(glm::vec3(50.0f, 0.0f, 0.0f));
	dataSlidingCircle->translationMatrixLoc = glGetUniformLocation(dataSlidingCircle->shader.getProgramID(), "translationMatrix");
	glUniformMatrix4fv(dataSlidingCircle->translationMatrixLoc, 1, GL_FALSE, glm::value_ptr(dataSlidingCircle->translationMatrix));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	return dataSlidingCircle;
}

DataClickSlidingCirle* Game::CreateDataClickSlidingCircle(const glm::vec3 startPos, const glm::vec3 endPos, const int repeat)
{
	DataClickSlidingCirle* dataClickSlidingCircle = new DataClickSlidingCirle;

	glCreateVertexArrays(1, &dataClickSlidingCircle->vao);
	glBindVertexArray(dataClickSlidingCircle->vao);

	glGenBuffers(1, &dataClickSlidingCircle->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, dataClickSlidingCircle->vbo);

	dataClickSlidingCircle->center = startPos;
	GenDataCircle(dataClickSlidingCircle->points,
		dataClickSlidingCircle->indices,
		dataClickSlidingCircle->center,
		CIRCLE_INNER_RADIUS,
		CIRCLE_RADIUS,
		CIRCLE_OUTER_RADIUS,
		CIRCLE_RESOLUTION,
		COLOR_BACKGROUND,
		COLOR_SHRINK_CIRCLE,
		COLOR_CENTER);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, dataClickSlidingCircle->points.size() * sizeof(float), &(dataClickSlidingCircle->points[0]), GL_STATIC_DRAW));

	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &(dataClickSlidingCircle->ebo)));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataClickSlidingCircle->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataClickSlidingCircle->indices.size() * sizeof(unsigned int), &(dataClickSlidingCircle->indices[0]), GL_STATIC_DRAW));

	dataClickSlidingCircle->shader = Shader("src/shaders/clickSlidingBaseVertex.shader", "src/shaders/clickSlidingBaseFragment.shader");
	dataClickSlidingCircle->shader.useProgram();

	int uniformLocation = glGetUniformLocation(dataClickSlidingCircle->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	uniformLocation = glGetUniformLocation(dataClickSlidingCircle->shader.getProgramID(), "toOrigin");
	glm::mat4 toOrigin = glm::translate(glm::vec3(-1.0f) * dataClickSlidingCircle->center);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(toOrigin));

	uniformLocation = glGetUniformLocation(dataClickSlidingCircle->shader.getProgramID(), "fromOrigin");
	glm::mat4 fromOrigin = glm::translate(dataClickSlidingCircle->center);
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(fromOrigin));

	dataClickSlidingCircle->scaleMatrixLoc = glGetUniformLocation(dataClickSlidingCircle->shader.getProgramID(), "scaleMatrix");
	glm::mat4 scaleMatrix = glm::scale(glm::vec3(1.0f));
	glUniformMatrix4fv(dataClickSlidingCircle->scaleMatrixLoc, 1, GL_FALSE, glm::value_ptr(scaleMatrix));

	dataClickSlidingCircle->translationMatrix = glm::translate(glm::vec3(50.0f, 0.0f, 0.0f));
	dataClickSlidingCircle->translationMatrixLoc = glGetUniformLocation(dataClickSlidingCircle->shader.getProgramID(), "translationMatrix");
	glUniformMatrix4fv(dataClickSlidingCircle->translationMatrixLoc, 1, GL_FALSE, glm::value_ptr(dataClickSlidingCircle->translationMatrix));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	return dataClickSlidingCircle;
}

DataMenu* Game::CreateDataMenu()
{
	DataMenu* dataMenu = new DataMenu;

	ASSERT(glCreateVertexArrays(1, &dataMenu->vao));
	ASSERT(glBindVertexArray(dataMenu->vao));

	ASSERT(glGenBuffers(1, &dataMenu->vbo));
	ASSERT(glBindBuffer(GL_ARRAY_BUFFER, dataMenu->vbo));

	dataMenu->center = glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, 0.0f);


	GenDataTexture(dataMenu->points, dataMenu->indices, dataMenu->center, 100.0f, 100.0f);

	ASSERT(glBufferData(GL_ARRAY_BUFFER, dataMenu->points.size() * sizeof(float), &dataMenu->points[0], GL_STATIC_DRAW));
	ASSERT(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	ASSERT(glEnableVertexAttribArray(0));

	ASSERT(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	ASSERT(glEnableVertexAttribArray(1));

	ASSERT(glGenBuffers(1, &dataMenu->ebo));
	ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dataMenu->ebo));
	ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataMenu->indices.size() * sizeof(unsigned int), &dataMenu->indices[0], GL_STATIC_DRAW));

	dataMenu->shader = Shader("src/shaders/menuVertex.shader", "src/shaders/menuFragment.shader");
	dataMenu->shader.useProgram();

	dataMenu->toOrigin = glm::translate(glm::vec3(-1.0f) * dataMenu->center);
	dataMenu->fromOrigin = glm::translate(dataMenu->center);
	dataMenu->scaleMatrix = glm::scale(glm::vec3(1.0f));

	int uniformLocation = glGetUniformLocation(dataMenu->shader.getProgramID(), "orthoMatrix");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(orthoMatrix));

	uniformLocation = glGetUniformLocation(dataMenu->shader.getProgramID(), "toOrigin");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(dataMenu->toOrigin));
	
	uniformLocation = glGetUniformLocation(dataMenu->shader.getProgramID(), "fromOrigin");
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(dataMenu->fromOrigin));

	dataMenu->scaleMatrixLoc = glGetUniformLocation(dataMenu->shader.getProgramID(), "scaleMatrix");
	glUniformMatrix4fv(dataMenu->scaleMatrixLoc, 1, GL_FALSE, glm::value_ptr(dataMenu->scaleMatrix));

	int x, y, n;
	unsigned char* imageData = stbi_load("res/textures/play.png", &x, &y, &n, 0);

	if (!imageData)
	{
		std::cout << "Failed to load image data!" << std::endl;
		__debugbreak();
	}

	ASSERT(glGenTextures(1, &dataMenu->textureID));
	ASSERT(glBindTexture(GL_TEXTURE_2D, dataMenu->textureID));
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
	glUseProgram(0);

	return dataMenu;
}

void Game::DrawBasicCircle(BasicCircle* circle)
{
	// shrink circle
	glBindVertexArray(circle->dataShrinkCircle->vao);
	circle->dataShrinkCircle->shader.useProgram();

	if (circle->dataShrinkCircle->shrinkFactor > 1)
	{
		circle->dataShrinkCircle->shrinkFactor -= circle->dataShrinkCircle->shrinkSpeed;
		float scale = circle->dataShrinkCircle->shrinkFactor;
		circle->dataShrinkCircle->shrinkMatrix = glm::scale(glm::vec3(scale, scale, scale));
		glUniformMatrix4fv(circle->dataShrinkCircle->shrinkMatrixLoc, 1, GL_FALSE, glm::value_ptr(circle->dataShrinkCircle->shrinkMatrix));
	}
	else if (circle->dataShrinkCircle->shrinkFactor > 0.75)
	{
		circle->dataShrinkCircle->shrinkFactor -= circle->dataShrinkCircle->shrinkSpeed;
	}
	else // destroy circle
	{
		if (entity_buffer.size() != 1 && entity_buffer[1]->type == ENTITY_TYPE::SLIDER)
		{
			((Slider*)entity_buffer[1])->score = SCORE::FAIL;
		}
		else
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::FAIL));

		delete circle->dataShrinkCircle;
		delete circle->dataStaticCircle;
		delete circle->dataTextureCircle;
		delete entity_buffer[0];
		entity_buffer.pop_front();
		return;
	}

	ASSERT(glDrawElements(GL_TRIANGLES, circle->dataShrinkCircle->indices.size(), GL_UNSIGNED_INT, (void*)0));

	// static circle
	glBindVertexArray(circle->dataStaticCircle->vao);
	circle->dataStaticCircle->shader.useProgram();
	glDrawElements(GL_TRIANGLES, circle->dataStaticCircle->indices.size(), GL_UNSIGNED_INT, (void*)0);

	// texture
	glBindVertexArray(circle->dataTextureCircle->vao);
	circle->dataTextureCircle->shader.useProgram();
	glBindTexture(GL_TEXTURE_2D, circle->dataTextureCircle->textureID);
	glDrawElements(GL_TRIANGLES, circle->dataTextureCircle->indices.size(), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Game::DrawTextureScore(DataTextureScore* score)
{
	glBindVertexArray(score->vao);
	score->shader.useProgram();
	glBindTexture(GL_TEXTURE_2D, score->textureID);

	if (score->alpha > 0)
	{
		score->alpha -= CIRCLE_SHRINK_SPEED;
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
	glUseProgram(0);

}

void Game::OnEventBasicCircle(BasicCircle*& basicCircle, int key, int action, double x, double y)
{

	if (action == GLFW_RELEASE)
		return;

	BasicCircle* circle = (BasicCircle*)entity_buffer[0];

	bool in_range = x >= circle->dataShrinkCircle->center.x - 400.0f && x <= circle->dataShrinkCircle->center.x + 400.0f &&
		y >= circle->dataShrinkCircle->center.y - 400.0f && y <= circle->dataShrinkCircle->center.y + 400.0f;

	bool in_circle = y <= sqrt(CIRCLE_RADIUS * CIRCLE_RADIUS - pow(x - circle->dataShrinkCircle->center.x, 2)) + circle->dataShrinkCircle->center.y &&
		y >= circle->dataShrinkCircle->center.y - sqrt(CIRCLE_RADIUS * CIRCLE_RADIUS - pow(x - circle->dataShrinkCircle->center.x, 2));


	if (entity_buffer.size() > 1 && entity_buffer[1]->type == ENTITY_TYPE::SLIDER) // for slider initial circles: sends score data to slider instead of drawing score texture
	{
		if (!in_range) // if out of bounds, ignore input
			return;
		else if (!in_circle) 
		{
			((Slider*)entity_buffer[1])->score = SCORE::FAIL;
		}
		else if (circle->dataShrinkCircle->shrinkFactor < 1.2f && circle->dataShrinkCircle->shrinkFactor > 1.0f)
		{
			((Slider*)entity_buffer[1])->score = SCORE::SUCCESS;
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}

		else if (circle->dataShrinkCircle->shrinkFactor < 1.4f && circle->dataShrinkCircle->shrinkFactor > 1.2f)
		{
			((Slider*)entity_buffer[1])->score = SCORE::HUNDRED;
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}
		else if (circle->dataShrinkCircle->shrinkFactor < 1.0f && circle->dataShrinkCircle->shrinkFactor > 0.8f)
		{
			((Slider*)entity_buffer[1])->score = SCORE::HUNDRED;
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}

		else if (circle->dataShrinkCircle->shrinkFactor < 1.6 && circle->dataShrinkCircle->shrinkFactor > 1.4f)
		{
			((Slider*)entity_buffer[1])->score = SCORE::FIFTY;
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}
		else if (circle->dataShrinkCircle->shrinkFactor < 0.8f && circle->dataShrinkCircle->shrinkFactor > 0.6f)
		{
			((Slider*)entity_buffer[1])->score = SCORE::FIFTY;
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}

		else
		{
			((Slider*)entity_buffer[1])->score = SCORE::FAIL;
		}
	}

	else // normal circle
	{
		if (!in_range) // if out of bounds, ignore input
			return;
		else if (!in_circle)
		{
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::FAIL));
			sound_engine->play2D("res/audio/break_sound/break_sound_edited.ogg");
		}
		else if (circle->dataShrinkCircle->shrinkFactor < 1.2f && circle->dataShrinkCircle->shrinkFactor > 1.0f) // success
		{
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}

		else if (circle->dataShrinkCircle->shrinkFactor < 1.4f && circle->dataShrinkCircle->shrinkFactor > 1.2f)
		{
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::HUNDRED));
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}
		else if (circle->dataShrinkCircle->shrinkFactor < 1.0f && circle->dataShrinkCircle->shrinkFactor > 0.8f)
		{
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::HUNDRED));
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}

		else if (circle->dataShrinkCircle->shrinkFactor < 1.6 && circle->dataShrinkCircle->shrinkFactor > 1.4f)
		{
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::FIFTY));
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}
		else if (circle->dataShrinkCircle->shrinkFactor < 0.8f && circle->dataShrinkCircle->shrinkFactor > 0.6f)
		{
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::FIFTY));
			sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");
		}

		else
		{
			score_entity_buffer.push_back(CreateDataTextureScore(circle->dataShrinkCircle->center, SCORE::FAIL));
			sound_engine->play2D("res/audio/break_sound/break_sound_edited.ogg");
		}
	}

	delete circle->dataShrinkCircle;
	delete circle->dataStaticCircle;
	delete circle->dataTextureCircle;
	delete entity_buffer[0];
	entity_buffer.pop_front();
}

void Game::OnEventSlider(Slider*& slider, int key, int action, double x, double y)
{
	// slider destruction, -1 for x cursor position means slider expired
	if (slider->dataClickSlidingCircle->repeatCounter > slider->dataSlider->repeat || x == -1)
	{
		if (slider->score != SCORE::SUCCESS) // if success, no texture needed
		{
			if (slider->dataSlider->repeat % 2 == 0)
				score_entity_buffer.push_back(CreateDataTextureScore(slider->dataSlider->endPos, slider->score));
			else
				score_entity_buffer.push_back(CreateDataTextureScore(slider->dataSlider->center, slider->score));
			sound_engine->play2D("res/audio/break_sound/break_sound_edited.ogg");
		}
		sound_engine->play2D("res/audio/hit_sound/hit_sound_edited.ogg");

		delete slider->dataSlider;
		delete slider->dataSlidingCircle;
		delete slider->dataClickSlidingCircle;
		delete entity_buffer[0];
		entity_buffer.pop_front();
		return;
	}

	glBindVertexArray(slider->dataClickSlidingCircle->vao);
	slider->dataClickSlidingCircle->shader.useProgram();

	slider->dataClickSlidingCircle->scaleMatrix = glm::scale(glm::vec3(slider->dataClickSlidingCircle->scaleFactor));
	glUniformMatrix4fv(slider->dataClickSlidingCircle->scaleMatrixLoc, 1, GL_FALSE, glm::value_ptr(slider->dataClickSlidingCircle->scaleMatrix));
	
	// score deduction for releasing slider
	if (slider->dataClickSlidingCircle->scaleFactor < 1.0f && slider->score > SCORE::FAIL)
	{
		slider->score = SCORE::FAIL;
	}

	glBindVertexArray(0);
	slider->dataClickSlidingCircle->shader.unBind();
}

void Game::DrawSlider(Slider* slider)
{
	// draw base slider outline
	glBindVertexArray(slider->dataSlider->vao);
	slider->dataSlider->shader.useProgram();
	glDrawElements(GL_TRIANGLES, slider->dataSlider->indices.size(), GL_UNSIGNED_INT, (void*)0);

	
	// slider destruction
	if (slider->dataClickSlidingCircle->repeatCounter > slider->dataSlider->repeat)
	{
		if (slider->score != SCORE::SUCCESS) // if success, no texture needed
		{
			if (slider->dataSlider->repeat % 2 == 0)
				score_entity_buffer.push_back(CreateDataTextureScore(slider->dataSlider->endPos, slider->score));
			else
				score_entity_buffer.push_back(CreateDataTextureScore(slider->dataSlider->center, slider->score));
		}

		delete slider->dataSlider;
		delete slider->dataSlidingCircle;
		delete slider->dataClickSlidingCircle;
		delete entity_buffer[0];
		entity_buffer.pop_front();
		return;
	}

	// slider shrink/expand on click
	if (keyHold)
	{
		if (slider->dataClickSlidingCircle->scaleFactor < 1.5f)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			y = SCREEN_HEIGHT - y;

			bool in_circle = y <= sqrt(CIRCLE_RADIUS * CIRCLE_RADIUS - pow(x - (slider->dataSlidingCircle->translateXPos + slider->dataSlidingCircle->center.x), 2)) + (slider->dataSlidingCircle->translateYPos + slider->dataSlidingCircle->center.y) &&
				y >= (slider->dataSlidingCircle->translateYPos + slider->dataSlidingCircle->center.y) - sqrt(CIRCLE_RADIUS * CIRCLE_RADIUS - pow(x - (slider->dataSlidingCircle->translateXPos + slider->dataSlidingCircle->center.x), 2));

			if (in_circle)
				slider->dataClickSlidingCircle->scaleFactor += CIRCLE_SHRINK_SPEED * 3.0f;
		}
	}
	else if (slider->dataClickSlidingCircle->scaleFactor > 1.0f)
		slider->dataClickSlidingCircle->scaleFactor -= CIRCLE_SHRINK_SPEED;

	glBindVertexArray(slider->dataClickSlidingCircle->vao);
	slider->dataClickSlidingCircle->shader.useProgram();

	slider->dataClickSlidingCircle->scaleMatrix = glm::scale(glm::vec3(slider->dataClickSlidingCircle->scaleFactor));
	glUniformMatrix4fv(slider->dataClickSlidingCircle->scaleMatrixLoc, 1, GL_FALSE, glm::value_ptr(slider->dataClickSlidingCircle->scaleMatrix));

	// score deduction for releasing slider
	if (slider->dataClickSlidingCircle->scaleFactor < 1.0f && slider->score > SCORE::FAIL)
	{
		slider->score = SCORE::FAIL;
	}

	glBindVertexArray(0);
	slider->dataClickSlidingCircle->shader.unBind();



	// draw moving base circle
	if (((Slider*)entity_buffer[0])->dataSlider->vao == slider->dataSlider->vao) // after the inital circle is finished; ensures following sliders don't start drawing
	{
		// base circle draw
		glBindVertexArray(slider->dataSlidingCircle->vao);
		slider->dataSlidingCircle->shader.useProgram();

		if (slider->dataSlidingCircle->repeatCounter <= slider->dataSlider->repeat)
		{
			float x, y;

			if (slider->dataSlider->useYAxis) // use y axis if slope is too large to slow down slider
			{
				if (!slider->dataSlider->endIsUnderStart)
				{
					if (slider->dataSlidingCircle->repeatCounter % 2 == 0)
						slider->dataSlidingCircle->translateYPos += SLIDER_SPEED;
					else
						slider->dataSlidingCircle->translateYPos -= SLIDER_SPEED;
				}
				else // reverse above
				{
					if (slider->dataSlidingCircle->repeatCounter % 2 == 0)
						slider->dataSlidingCircle->translateYPos -= SLIDER_SPEED;
					else
						slider->dataSlidingCircle->translateYPos += SLIDER_SPEED;
				}
				y = slider->dataSlidingCircle->translateYPos;
				x = slider->dataSlidingCircle->translateXPos = slider->dataSlidingCircle->translateYPos / slider->dataSlider->slope;
			}
			else
			{
				if (slider->dataSlider->endIsLeftOfStart)
				{
					if (slider->dataSlidingCircle->repeatCounter % 2 == 0)
						slider->dataSlidingCircle->translateXPos -= SLIDER_SPEED;
					else
						slider->dataSlidingCircle->translateXPos += SLIDER_SPEED;
				}
				else // reverse above
				{
					if (slider->dataSlidingCircle->repeatCounter % 2 == 0)
						slider->dataSlidingCircle->translateXPos += SLIDER_SPEED;
					else
						slider->dataSlidingCircle->translateXPos -= SLIDER_SPEED;
				}
				y = slider->dataSlider->slope * slider->dataSlidingCircle->translateXPos;
				x = slider->dataSlidingCircle->translateXPos;
			}

			slider->dataSlidingCircle->translationMatrix = glm::translate(glm::vec3(x, y, 0.0f));

			glUniformMatrix4fv(slider->dataSlidingCircle->translationMatrixLoc, 1, GL_FALSE, glm::value_ptr(slider->dataSlidingCircle->translationMatrix));

			if (!slider->dataSlider->endIsLeftOfStart && (x + slider->dataSlider->center.x > slider->dataSlider->endPos.x || x + slider->dataSlider->center.x < slider->dataSlider->center.x))
				slider->dataSlidingCircle->repeatCounter++;
			else if (slider->dataSlider->endIsLeftOfStart && (x + slider->dataSlider->center.x < slider->dataSlider->endPos.x || x + slider->dataSlider->center.x > slider->dataSlider->center.x))
				slider->dataSlidingCircle->repeatCounter++;

		}

		glDrawElements(GL_TRIANGLES, slider->dataSlidingCircle->indices.size(), GL_UNSIGNED_INT, (void*)0);


		// click circle draw
		glBindVertexArray(slider->dataClickSlidingCircle->vao);
		slider->dataClickSlidingCircle->shader.useProgram();

		if (slider->dataClickSlidingCircle->repeatCounter <= slider->dataSlider->repeat)
		{
			float x, y;

			if (slider->dataSlider->useYAxis) // use y axis if slope is too large to slow down slider
			{
				if (!slider->dataSlider->endIsUnderStart)
				{
					if (slider->dataClickSlidingCircle->repeatCounter % 2 == 0)
						slider->dataClickSlidingCircle->translateYPos += SLIDER_SPEED;
					else
						slider->dataClickSlidingCircle->translateYPos -= SLIDER_SPEED;
				}
				else // reverse above
				{
					if (slider->dataClickSlidingCircle->repeatCounter % 2 == 0)
						slider->dataClickSlidingCircle->translateYPos -= SLIDER_SPEED;
					else
						slider->dataClickSlidingCircle->translateYPos += SLIDER_SPEED;
				}
				y = slider->dataClickSlidingCircle->translateYPos;
				x = slider->dataClickSlidingCircle->translateXPos = slider->dataClickSlidingCircle->translateYPos / slider->dataSlider->slope;
			}
			else
			{
				if (slider->dataSlider->endIsLeftOfStart)
				{
					if (slider->dataClickSlidingCircle->repeatCounter % 2 == 0)
						slider->dataClickSlidingCircle->translateXPos -= SLIDER_SPEED;
					else
						slider->dataClickSlidingCircle->translateXPos += SLIDER_SPEED;
				}
				else // reverse above
				{
					if (slider->dataClickSlidingCircle->repeatCounter % 2 == 0)
						slider->dataClickSlidingCircle->translateXPos += SLIDER_SPEED;
					else
						slider->dataClickSlidingCircle->translateXPos -= SLIDER_SPEED;
				}
				y = slider->dataSlider->slope * slider->dataClickSlidingCircle->translateXPos;
				x = slider->dataClickSlidingCircle->translateXPos;
			}

			slider->dataClickSlidingCircle->translationMatrix = glm::translate(glm::vec3(x, y, 0.0f));

			glUniformMatrix4fv(slider->dataClickSlidingCircle->translationMatrixLoc, 1, GL_FALSE, glm::value_ptr(slider->dataClickSlidingCircle->translationMatrix));

			if (!slider->dataSlider->endIsLeftOfStart && (x + slider->dataSlider->center.x > slider->dataSlider->endPos.x || x + slider->dataSlider->center.x < slider->dataSlider->center.x))
				slider->dataClickSlidingCircle->repeatCounter++;
			else if (slider->dataSlider->endIsLeftOfStart && (x + slider->dataSlider->center.x < slider->dataSlider->endPos.x || x + slider->dataSlider->center.x > slider->dataSlider->center.x))
				slider->dataClickSlidingCircle->repeatCounter++;

		}

		glDrawElements(GL_TRIANGLES, slider->dataClickSlidingCircle->indices.size(), GL_UNSIGNED_INT, (void*)0);
	}

	if (slider->dataSlidingCircle->repeatCounter > slider->dataSlider->repeat) // slider expired
	{
		OnEvent(GLFW_KEY_Z, NULL, -1, -1);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Game::DrawMenu()
{
	glBindVertexArray(menu->vao);
	menu->shader.useProgram();
	glBindTexture(GL_TEXTURE_2D, menu->textureID);

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	y = SCREEN_HEIGHT - y;

	bool on_play = x <= menu->center.x + 125.0f && x >= menu->center.x - 125.0f &&
		y <= menu->center.y + 125.0f && y >= menu->center.y - 125.0f;

	if (on_play && menu->scaleFactor < 1.1f)
	{
		menu->scaleFactor += CIRCLE_SHRINK_SPEED * 4.0f;
		menu->scaleMatrix = glm::scale(glm::vec3(menu->scaleFactor));
		glUniformMatrix4fv(menu->scaleMatrixLoc, 1, GL_FALSE, glm::value_ptr(menu->scaleMatrix));
	}
	else if (menu->scaleFactor > 1.0f && !on_play)
	{
		menu->scaleFactor -= CIRCLE_SHRINK_SPEED * 4.0f;
		menu->scaleMatrix = glm::scale(glm::vec3(menu->scaleFactor));
		glUniformMatrix4fv(menu->scaleMatrixLoc, 1, GL_FALSE, glm::value_ptr(menu->scaleMatrix));
	}

	glDrawElements(GL_TRIANGLES, menu->indices.size(), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(menu_logo->vao);
	menu_logo->shader.useProgram();
	glBindTexture(GL_TEXTURE_2D, menu_logo->textureID);
	glDrawElements(GL_TRIANGLES, menu_logo->indices.size(), GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

#include "BeatMap.h" // circular dependency of BeatMap and Game

void Game::Draw()
{
	if (inMenu)
	{
		DrawMenu();
		return;
	}

	beatMap->Map();

	for (int i = entity_buffer.size() - 1; i >= 0; i--)
	{
		if (entity_buffer[i]->type == ENTITY_TYPE::BASIC)
			DrawBasicCircle((BasicCircle*)entity_buffer[i]);
		else if (entity_buffer[i]->type == ENTITY_TYPE::SLIDER)
		{
			DrawSlider((Slider*)entity_buffer[i]);
		}
		else
		{
			std::cout << "Unknown Entity" << std::endl;
			__debugbreak();
		}
	}

	for (int i = 0; i < score_entity_buffer.size(); i++)
	{
		DrawTextureScore(score_entity_buffer[i]);
	}
}

void Game::OnEvent(int key, int action, double x, double y)
{

	if (key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	// menu
	if (inMenu)
	{
		bool on_play = x <= menu->center.x + 125.0f && x >= menu->center.x - 125.0f &&
			y <= menu->center.y + 125.0f && y >= menu->center.y - 125.0f;

		if (on_play && action == GLFW_PRESS)
		{
			inMenu = false;
			sound_engine->play2D("res/audio/believer/believer_delay_edited.ogg");
			beatMap = new BeatMap(this, BPM, BEATS_PER_BAR);
		}
		return;
	}

	// game
	else if (key != GLFW_KEY_Z && key != GLFW_KEY_X && key != GLFW_MOUSE_BUTTON_1)
		return;

	if (entity_buffer.size() == 0)
		return;

	if (action == GLFW_REPEAT)
		return;
	else if (action == GLFW_PRESS)
	{
		keyHold++;
	}
	else if (action == GLFW_RELEASE && keyHold > 0)
		keyHold--;

	if (entity_buffer.size() == 0 && action == GLFW_RELEASE) // crashes when clicking start button
		return;

	switch (entity_buffer[0]->type)
	{
	case ENTITY_TYPE::BASIC:
		{
			BasicCircle* basicCircle = (BasicCircle*)entity_buffer[0];
			OnEventBasicCircle(basicCircle, key, action, x, y);
			break;
		}

	case ENTITY_TYPE::SLIDER:
		{
			Slider* slider = (Slider*)entity_buffer[0];
			OnEventSlider(slider, key, action, x, y);
			break;
		}
	}
}

void Game::GenDataCircle(std::vector<float>& points, 
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

void Game::GenDataTexture(std::vector<float>& points, 
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

void Game::GenDataSlider(std::vector<float>& points, 
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

	double x = endPos.x - startPos.x;
	double y = endPos.y - startPos.y;
	double angle = atan(y / x);

	if (endIsLeftOfStart)
	{
		angle += M_PI;
	}

	// add slider
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
		float x = innerRadius * cos(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.x;
		float y = innerRadius * sin(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);

		// middle points
		x = middleRadius * cos(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.x;
		y = middleRadius * sin(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(circleColor.x);
		points.push_back(circleColor.y);
		points.push_back(circleColor.z);
		points.push_back(circleColor.w);

		// outer points
		x = outerRadius * cos(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.x;
		y = outerRadius * sin(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.y;
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
			float x = innerRadius * cos(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.x;
			float y = innerRadius * sin(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(backgroundColor.x);
			points.push_back(backgroundColor.y);
			points.push_back(backgroundColor.z);
			points.push_back(backgroundColor.w);

			// middle points
			x = middleRadius * cos(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.x;
			y = middleRadius * sin(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(circleColor.x);
			points.push_back(circleColor.y);
			points.push_back(circleColor.z);
			points.push_back(circleColor.w);

			// outer points
			x = outerRadius * cos(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.x;
			y = outerRadius * sin(i * M_PI / resolution + angle + M_PI / 2.0) + startPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(backgroundColor.x);
			points.push_back(backgroundColor.y);
			points.push_back(backgroundColor.z);
			points.push_back(backgroundColor.w);
		}
	}

	// second circle
	if (points.size() % 7 != 0) { // should always be divisible by 7
		std::cout << "Generate points size error!" << std::endl;
		__debugbreak();
	}

	int secondCircleStartIndex = points.size() / 7;

	points.push_back(endPos.x);
	points.push_back(endPos.y);
	points.push_back(endPos.z);
	points.push_back(circleCenterColor.x);
	points.push_back(circleCenterColor.y);
	points.push_back(circleCenterColor.z);
	points.push_back(circleCenterColor.w);

	for (int i = 0; i < resolution; i++)
	{
		// inner points
		float x = innerRadius * cos(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.x;
		float y = innerRadius * sin(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);

		// middle points
		x = middleRadius * cos(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.x;
		y = middleRadius * sin(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(circleColor.x);
		points.push_back(circleColor.y);
		points.push_back(circleColor.z);
		points.push_back(circleColor.w);

		// outer points
		x = outerRadius * cos(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.x;
		y = outerRadius * sin(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.y;
		points.push_back(x);
		points.push_back(y);
		points.push_back(0.0f); // z coordinate
		points.push_back(backgroundColor.x);
		points.push_back(backgroundColor.y);
		points.push_back(backgroundColor.z);
		points.push_back(backgroundColor.w);

		// center triangle (0 1 4)
		indices.push_back(secondCircleStartIndex);
		indices.push_back(secondCircleStartIndex + i * 3 + 1);
		indices.push_back(secondCircleStartIndex + i * 3 + 4);

		// inner triangle 1 (1 2 4)		
		indices.push_back(secondCircleStartIndex + i * 3 + 1);
		indices.push_back(secondCircleStartIndex + i * 3 + 2);
		indices.push_back(secondCircleStartIndex + i * 3 + 4);

		// inner triangle 2 (2 4 5)
		indices.push_back(secondCircleStartIndex + i * 3 + 2);
		indices.push_back(secondCircleStartIndex + i * 3 + 4);
		indices.push_back(secondCircleStartIndex + i * 3 + 5);

		// outer triangle 1 (2 3 5)
		indices.push_back(secondCircleStartIndex + i * 3 + 2);
		indices.push_back(secondCircleStartIndex + i * 3 + 3);
		indices.push_back(secondCircleStartIndex + i * 3 + 5);

		// outer triangle 2 (3 5 6)
		indices.push_back(secondCircleStartIndex + i * 3 + 3);
		indices.push_back(secondCircleStartIndex + i * 3 + 5);
		indices.push_back(secondCircleStartIndex + i * 3 + 6);

		if (i == resolution - 1)
		{
			i++;
			// inner points
			float x = innerRadius * cos(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.x;
			float y = innerRadius * sin(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(backgroundColor.x);
			points.push_back(backgroundColor.y);
			points.push_back(backgroundColor.z);
			points.push_back(backgroundColor.w);

			// middle points
			x = middleRadius * cos(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.x;
			y = middleRadius * sin(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(circleColor.x);
			points.push_back(circleColor.y);
			points.push_back(circleColor.z);
			points.push_back(circleColor.w);

			// outer points
			x = outerRadius * cos(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.x;
			y = outerRadius * sin(i * M_PI / resolution + angle - M_PI / 2.0) + endPos.y;
			points.push_back(x);
			points.push_back(y);
			points.push_back(0.0f); // z coordinate
			points.push_back(backgroundColor.x);
			points.push_back(backgroundColor.y);
			points.push_back(backgroundColor.z);
			points.push_back(backgroundColor.w);
		}
	}

	// slider pipe
	if (points.size() % 7 != 0) { // should always be divisible by 7
		std::cout << "Generate points size error!" << std::endl;
		__debugbreak();
	}

	int sliderPipeStartIndex = points.size() / 7;

	// 0
	points.push_back(startPos.x);
	points.push_back(startPos.y);
	points.push_back(startPos.z);
	points.push_back(circleCenterColor.x);
	points.push_back(circleCenterColor.y);
	points.push_back(circleCenterColor.z);
	points.push_back(circleCenterColor.w);


	// 1 2 3
	x = innerRadius * cos(angle + M_PI / 2.0) + startPos.x;
	y = innerRadius * sin(angle + M_PI / 2.0) + startPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	x = middleRadius * cos(angle + M_PI / 2.0) + startPos.x;
	y = middleRadius * sin(angle + M_PI / 2.0) + startPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(circleColor.x);
	points.push_back(circleColor.y);
	points.push_back(circleColor.z);
	points.push_back(circleColor.w);

	x = outerRadius * cos(angle + M_PI / 2.0) + startPos.x;
	y = outerRadius * sin(angle + M_PI / 2.0) + startPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);


	// 4 5 6
	x = innerRadius * cos(angle - M_PI / 2.0) + startPos.x;
	y = innerRadius * sin(angle - M_PI / 2.0) + startPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	x = middleRadius * cos(angle - M_PI / 2.0) + startPos.x;
	y = middleRadius * sin(angle - M_PI / 2.0) + startPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(circleColor.x);
	points.push_back(circleColor.y);
	points.push_back(circleColor.z);
	points.push_back(circleColor.w);

	x = outerRadius * cos(angle - M_PI / 2.0) + startPos.x;
	y = outerRadius * sin(angle - M_PI / 2.0) + startPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	// 7
	points.push_back(endPos.x);
	points.push_back(endPos.y);
	points.push_back(endPos.z);
	points.push_back(circleCenterColor.x);
	points.push_back(circleCenterColor.y);
	points.push_back(circleCenterColor.z);
	points.push_back(circleCenterColor.w);

	// 8 9 10
	x = innerRadius * cos(angle + M_PI / 2.0) + endPos.x;
	y = innerRadius * sin(angle + M_PI / 2.0) + endPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	x = middleRadius * cos(angle + M_PI / 2.0) + endPos.x;
	y = middleRadius * sin(angle + M_PI / 2.0) + endPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(circleColor.x);
	points.push_back(circleColor.y);
	points.push_back(circleColor.z);
	points.push_back(circleColor.w);

	x = outerRadius * cos(angle + M_PI / 2.0) + endPos.x;
	y = outerRadius * sin(angle + M_PI / 2.0) + endPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	// 11 12 13
	x = innerRadius * cos(angle - M_PI / 2.0) + endPos.x;
	y = innerRadius * sin(angle - M_PI / 2.0) + endPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	x = middleRadius * cos(angle - M_PI / 2.0) + endPos.x;
	y = middleRadius * sin(angle - M_PI / 2.0) + endPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(circleColor.x);
	points.push_back(circleColor.y);
	points.push_back(circleColor.z);
	points.push_back(circleColor.w);

	x = outerRadius * cos(angle - M_PI / 2.0) + endPos.x;
	y = outerRadius * sin(angle - M_PI / 2.0) + endPos.y;
	points.push_back(x);
	points.push_back(y);
	points.push_back(0.0f);
	points.push_back(backgroundColor.x);
	points.push_back(backgroundColor.y);
	points.push_back(backgroundColor.z);
	points.push_back(backgroundColor.w);

	// "top" side
	indices.push_back(sliderPipeStartIndex);
	indices.push_back(sliderPipeStartIndex + 1);
	indices.push_back(sliderPipeStartIndex + 7);

	indices.push_back(sliderPipeStartIndex + 8);
	indices.push_back(sliderPipeStartIndex + 1);
	indices.push_back(sliderPipeStartIndex + 7);

	indices.push_back(sliderPipeStartIndex + 1);
	indices.push_back(sliderPipeStartIndex + 8);
	indices.push_back(sliderPipeStartIndex + 2);

	indices.push_back(sliderPipeStartIndex + 2);
	indices.push_back(sliderPipeStartIndex + 9);
	indices.push_back(sliderPipeStartIndex + 8);

	indices.push_back(sliderPipeStartIndex + 2);
	indices.push_back(sliderPipeStartIndex + 3);
	indices.push_back(sliderPipeStartIndex + 9);

	indices.push_back(sliderPipeStartIndex + 3);
	indices.push_back(sliderPipeStartIndex + 10);
	indices.push_back(sliderPipeStartIndex + 9);

	// "bottom" side
	indices.push_back(sliderPipeStartIndex);
	indices.push_back(sliderPipeStartIndex + 4);
	indices.push_back(sliderPipeStartIndex + 11);

	indices.push_back(sliderPipeStartIndex);
	indices.push_back(sliderPipeStartIndex + 7);
	indices.push_back(sliderPipeStartIndex + 11);

	indices.push_back(sliderPipeStartIndex + 4);
	indices.push_back(sliderPipeStartIndex + 5);
	indices.push_back(sliderPipeStartIndex + 12);

	indices.push_back(sliderPipeStartIndex + 4);
	indices.push_back(sliderPipeStartIndex + 11);
	indices.push_back(sliderPipeStartIndex + 12);

	indices.push_back(sliderPipeStartIndex + 5);
	indices.push_back(sliderPipeStartIndex + 6);
	indices.push_back(sliderPipeStartIndex + 13);

	indices.push_back(sliderPipeStartIndex + 5);
	indices.push_back(sliderPipeStartIndex + 12);
	indices.push_back(sliderPipeStartIndex + 13);

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
