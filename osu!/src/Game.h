#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "irrKlang.h"

#include <vector>
#include <deque>
#include <string>

#include <glm/glm.hpp>
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Assert.h"
#include "Shader.h"

#define M_PI 3.14159265358979323846
#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f

enum class ENTITY_TYPE
{
	BASIC = 0, SLIDER = 1, SPINNER = 2
};

enum class SCORE
{
	FAIL = 0, FIFTY = 1, HUNDRED = 2, SUCCESS = 3
};

struct CircleData
{
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	Shader shader;
	
	std::vector<float> points;
	std::vector<unsigned int> indices;
	glm::vec3 center;
};

struct StaticCircleData : public CircleData {};

struct TextureCircleData : public CircleData
{
	int index;
	unsigned int textureID;
};

struct ShrinkCircleData : public CircleData
{
	ShrinkCircleData(const float initSize, const float shrinkSpeed)
		:shrinkFactor(initSize), shrinkSpeed(shrinkSpeed), 
		shrinkMatrix(glm::mat4(1.0f)), toOrigin(glm::mat4(1.0f)), fromOrigin(glm::mat4(1.0f)) {}

	float shrinkFactor;
	float shrinkSpeed;
	int shrinkMatrixLoc;
	glm::mat4 shrinkMatrix;
	glm::mat4 toOrigin;
	glm::mat4 fromOrigin;
};

struct TextureScoreData : public CircleData
{
	TextureScoreData() : alpha(1.0f) {}

	SCORE score;
	unsigned int textureID;

	int colorUniformLoc;
	float alpha;
};

struct StraightSliderData : public CircleData
{
	glm::vec3 endPos;
	bool reverse;
};

struct Entity
{
	ENTITY_TYPE type;
};

struct BasicCircle : public Entity
{
	StaticCircleData* staticCircleData;
	ShrinkCircleData* shrinkCircleData;
	TextureCircleData* textureCircleData;
};

struct StraightSlider : public Entity
{
	BasicCircle* basicCircle;
	StraightSliderData* straightSliderData;
};

class BeatMap;

class Game
{
	friend BeatMap;
private:

	irrklang::ISoundEngine* sound_engine;

	std::deque<Entity*> entity_buffer;
	std::deque<TextureScoreData*> score_entity_buffer;

	const float CIRCLE_RADIUS;
	const int CIRCLE_RESOLUTION;
	const glm::vec4 COLOR_BACKGROUND;
	const glm::vec4 COLOR_SHRINK_CIRCLE;
	const glm::vec4 COLOR_STATIC_CIRCLE;
	const glm::vec4 COLOR_CENTER;
	const float CIRCLE_INIT_SIZE;
	const float CIRCLE_SHRINK_SPEED;
	glm::mat4 orthoMatrix;

	void CreateBasicCircle(const glm::vec3 center, const int index);
	ShrinkCircleData* CreateShrinkCircleData(const glm::vec3 center);
	StaticCircleData* CreateStaticCircleData(const glm::vec3 center);
	TextureCircleData* CreateTextureCircleData(const glm::vec3 center, const int index);

	TextureScoreData* CreateTextureScoreData(const glm::vec3 center, SCORE score);

	StraightSliderData* CreateStraightSliderData(const glm::vec3 startPos, const glm::vec3 endPos, const bool repeat = false);


	void GenCircleData(std::vector<float>& points,
		std::vector<unsigned int>& indices,
		const glm::vec3 center,
		const float innerRadius,
		const float middleRadius,
		const float outerRadius,
		const int resolution,
		const glm::vec4 backgroundColor,
		const glm::vec4 circleColor,
		const glm::vec4 circleCenterColor);

	void GenTextureData(std::vector<float>& points,
		std::vector<unsigned int>& indices,
		const glm::vec3 center,
		const float width = 20.0f,
		const float height = 30.0f);
	void GenStraightSliderData(std::vector<float>& points,
		std::vector<unsigned int>& indices,
		const glm::vec3 startPos,
		const glm::vec3 endPos, 
		const float innerRadius,
		const float middleRadius,
		const float outerRadius,
		const int resolution,
		const glm::vec4 backgroundColor,
		const glm::vec4 circleColor,
		const glm::vec4 circleCenterColor);

	void BasicCircleDraw(BasicCircle* circle);
	void TextureScoreDraw(TextureScoreData* score);

public:
	Game(const float circleRadius = 60.0f, 
		const int circleResolution = 72, 
		glm::vec4 color_background = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 
		glm::vec4 color_shrink_circle = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4 color_static_circle = glm::vec4(0.8f, 0.5f, 1.0f, 1.0f),
		glm::vec4 color_center = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), 
		const float circle_init_size = 2.5f, 
		const float circle_shrink_speed = 0.01f);
	~Game();

	void Draw();
	
	void OnEvent(int key, int action, double x, double y);

	static void KeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseInput(GLFWwindow* window, int button, int action, int mods);
};

