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
#define SCREEN_CENTER_X SCREEN_WIDTH / 2.0f
#define SCREEN_CENTER_Y SCREEN_HEIGHT / 2.0f
#define SCREEN_CENTER SCREEN_CENTER_X, SCREEN_CENTER_Y
#define SCREEN_CENTER(x, y) SCREEN_CENTER_X + x, SCREEN_CENTER_Y + y



#ifdef LAPTOP
#define PLATFORM_SHRINK_SPEED 0.0003f
#define PLATFORM_SLIDER_SPEED 0.1f
#else
#define PLATFORM_SHRINK_SPEED 0.0115f
#define PLATFORM_SLIDER_SPEED 4.0f
#endif


enum class ENTITY_TYPE
{
	BASIC = 0, SLIDER = 1, SPINNER = 2
};

enum class SCORE
{
	FAIL = 0, FIFTY = 1, HUNDRED = 2, SUCCESS = 3
};

struct DataCircle
{
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	Shader shader;
	
	std::vector<float> points;
	std::vector<unsigned int> indices;
	glm::vec3 center;
};

struct DataStaticCircle : public DataCircle {};

struct DataTextureCircle : public DataCircle
{
	int index;
	unsigned int textureID;
};

struct DataShrinkCircle : public DataCircle
{
	DataShrinkCircle(const float initSize, const float shrinkSpeed)
		:shrinkFactor(initSize), shrinkSpeed(shrinkSpeed), 
		shrinkMatrix(glm::mat4(1.0f)), toOrigin(glm::mat4(1.0f)), fromOrigin(glm::mat4(1.0f)) {}

	float shrinkFactor;
	float shrinkSpeed;
	int shrinkMatrixLoc;
	glm::mat4 shrinkMatrix;
	glm::mat4 toOrigin;
	glm::mat4 fromOrigin;
};

struct DataTextureScore : public DataCircle
{
	DataTextureScore() : alpha(1.0f) {}

	SCORE score;
	unsigned int textureID;

	int colorUniformLoc;
	float alpha;
};

struct DataSlidingCircle : public DataCircle
{
	DataSlidingCircle() : translationMatrix(glm::mat4(1.0f)), translateXPos(0.0f), translateYPos(0.0f), repeatCounter(0) {};
	int translationMatrixLoc;
	glm::mat4 translationMatrix;
	float translateXPos, translateYPos;
	int repeatCounter;
};

struct DataClickSlidingCirle : public DataSlidingCircle
{
	DataClickSlidingCirle() : scaleMatrix(glm::mat4(1.0f)), scaleFactor(1.0f) {};
	int scaleMatrixLoc;
	glm::mat4 scaleMatrix;
	float scaleFactor;
};

struct DataSlider : public DataCircle
{
	glm::vec3 endPos;
	int repeat;
	double slope;
	bool useYAxis;
	bool endIsLeftOfStart;
	bool endIsUnderStart;

	DataTextureCircle* dataReverseArrowStart;
	DataTextureCircle* dataReverseArrowEnd;
};

struct DataMenu : public DataCircle
{
	DataMenu() : scaleMatrix(glm::mat4(1.0f)), scaleFactor(1.0f), toOrigin(glm::mat4(1.0f)), fromOrigin(glm::mat4(1.0f)), DataCircle() {}

	unsigned int textureID;
	int scaleMatrixLoc;
	glm::mat4 scaleMatrix;
	float scaleFactor;

	glm::mat4 toOrigin;
	glm::mat4 fromOrigin;
};

struct Entity
{
	ENTITY_TYPE type;
};

struct BasicCircle : public Entity
{
	DataStaticCircle* dataStaticCircle;
	DataShrinkCircle* dataShrinkCircle;
	DataTextureCircle* dataTextureCircle;
};

struct Slider : public Entity
{
	BasicCircle* basicCircle;
	DataSlider* dataSlider;
	DataSlidingCircle* dataSlidingCircle;
	DataClickSlidingCirle* dataClickSlidingCircle;
	SCORE score;
};

class BeatMap;

class Game
{
	friend BeatMap;
private:

	GLFWwindow* window;
	irrklang::ISoundEngine* sound_engine;
	BeatMap* beatMap;
	const double BPM;
	const int BEATS_PER_BAR;

	std::deque<Entity*> entity_buffer;
	std::deque<DataTextureScore*> score_entity_buffer;
	
	const float CIRCLE_INNER_RADIUS;
	const float CIRCLE_RADIUS;
	const float CIRCLE_OUTER_RADIUS;
	const int CIRCLE_RESOLUTION;
	const glm::vec4 COLOR_BACKGROUND;
	const glm::vec4 COLOR_SHRINK_CIRCLE;
	const glm::vec4 COLOR_STATIC_CIRCLE;
	const glm::vec4 COLOR_CENTER;
	const float CIRCLE_INIT_SIZE;
	const float CIRCLE_SHRINK_SPEED;
	const float SLIDER_SPEED;
	glm::mat4 orthoMatrix;

	int keyHold;

	bool inMenu;
	DataMenu* menu;
	DataTextureCircle* menu_logo;

	BasicCircle* CreateBasicCircle(const glm::vec3 center, const int index);
	DataShrinkCircle* CreateDataShrinkCircle(const glm::vec3 center);
	DataStaticCircle* CreateDataStaticCircle(const glm::vec3 center);
	DataTextureCircle* CreateDataTextureCircle(const glm::vec3 center, const int index, const glm::vec3 endPos = glm::vec3(-1.0f));

	DataTextureScore* CreateDataTextureScore(const glm::vec3 center, SCORE score);

	Slider* CreateSlider(const glm::vec3 startPos, const glm::vec3 endPos, const int index, const int repeat);
	DataSlider* CreateDataSlider(const glm::vec3 startPos, const glm::vec3 endPos, const int repeat);
	DataSlidingCircle* CreateDataSlidingCircle(const glm::vec3 startPos, const glm::vec3 endPos, const int repeat);
	DataClickSlidingCirle* CreateDataClickSlidingCircle(const glm::vec3 startPos, const glm::vec3 endPos, const int repeat);
	DataMenu* CreateDataMenu();

	void GenDataCircle(std::vector<float>& points,
		std::vector<unsigned int>& indices,
		const glm::vec3 center,
		const float innerRadius,
		const float middleRadius,
		const float outerRadius,
		const int resolution,
		const glm::vec4 backgroundColor,
		const glm::vec4 circleColor,
		const glm::vec4 circleCenterColor);

	void GenDataTexture(std::vector<float>& points,
		std::vector<unsigned int>& indices,
		const glm::vec3 center,
		const float width = 10.0f,
		const float height = 15.0f);
	void GenDataSlider(std::vector<float>& points,
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

	void DrawBasicCircle(BasicCircle* circle);
	void DrawTextureScore(DataTextureScore* score);
	void DrawSlider(Slider* slider);
	void DrawMenu();

	void OnEventBasicCircle(BasicCircle*& basicCircle, int key, int action, double x, double y);
	void OnEventSlider(Slider*& slider, int key, int action, double x, double y);

public:
	Game(GLFWwindow* win, 
		const double bpm, 
		const int beatsPerBar, 
		const float circleInnerRadius = 20.0f,
		const float circleRadius = 60.0f,
		const float circleOuterRadius = 70.0f,
		const int circleResolution = 72, 
		glm::vec4 color_background = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 
		glm::vec4 color_shrink_circle = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4 color_static_circle = glm::vec4(0.8f, 0.5f, 1.0f, 1.0f),
		glm::vec4 color_center = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), 
		const float circle_init_size = 2.5f, 
		const float circle_shrink_speed = PLATFORM_SHRINK_SPEED,
		const float slider_speed = PLATFORM_SLIDER_SPEED,
		const int hold = 0, 
		const bool menu = true);
	~Game();



	void Draw();
	
	void OnEvent(int key, int action, double x, double y);

	static void KeyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseInput(GLFWwindow* window, int button, int action, int mods);
};

