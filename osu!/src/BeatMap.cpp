#include "BeatMap.h"

BeatMap::BeatMap(Game* game, const double bpm, const int beatsPerBar)
	:game(game), BPM(bpm), BEATS_PER_BAR(beatsPerBar), entity_tracker(0)
{
	//game->sound_engine->play2D("res/audio/believer.ogg");

	// delay to match audio
	glfwSetTime(0);
	while (true)
	{
		if (glfwGetTime() > 0.3)
		{
			glfwSetTime(0);
			break;
		}
	}
}

void BeatMap::AddEntity(int tracker,
	const int bar,
	const float beat,
	ENTITY_TYPE type,
	glm::vec3 startPos,
	int index /*= 0*/,
	glm::vec3 endPos /*= glm::vec3(0.0f)*/,
	bool repeat /*= false*/)
{
	if (entity_tracker == tracker)
	{
		int currentBar;
		double currentBeat;
		CurrentBeat(currentBar, currentBeat);
		if (currentBar == bar && (currentBeat >= beat - 0.04 && currentBeat <= beat + 0.04)) // not very accurate
		{
			switch (type)
			{
			case ENTITY_TYPE::BASIC:
				game->CreateBasicCircle(startPos, index);
				break;
			case ENTITY_TYPE::SLIDER:
				game->CreateSlider(startPos, endPos, index, repeat);
				break;
			// other cases: spinners
			}
			entity_tracker++;
		}
	}
}

void BeatMap::CurrentBeat(int& bar, double& beat) const // calculate beat offset from circle shrink speed
{
	double netMinutes = glfwGetTime() / 60.0;
	double netBeats = netMinutes * BPM;
	bar = netBeats / BEATS_PER_BAR + 1;
	beat = netBeats - (bar - 1) * BEATS_PER_BAR + 1;
}

void BeatMap::Map()
{
	int tracker = 0;


	AddEntity(tracker++, 1, 1, ENTITY_TYPE::SLIDER, glm::vec3(300.0f, 300.0f, 0.0f), 1, glm::vec3(500.0f, 600.0f, 0.0f));
	//AddEntity(1, 2, ENTITY_TYPE::BASIC, glm::vec3(400.0f, 300.0f, 0.0f), 2, tracker++);
	//AddEntity(1, 3, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 300.0f, 0.0f), 3, tracker++);
	//AddEntity(1, 4, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 300.0f, 0.0f), 4, tracker++);

	//AddEntity(2, 1, ENTITY_TYPE::BASIC, glm::vec3(300.0f, 400.0f, 0.0f), 1, tracker++);
	//AddEntity(2, 2, ENTITY_TYPE::BASIC, glm::vec3(400.0f, 400.0f, 0.0f), 2, tracker++);
	//AddEntity(2, 3, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 400.0f, 0.0f), 3, tracker++);
	//AddEntity(2, 4, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 400.0f, 0.0f), 4, tracker++);

	//AddEntity(3, 1, ENTITY_TYPE::BASIC, glm::vec3(300.0f, 500.0f, 0.0f), 1, tracker++);
	//AddEntity(3, 2, ENTITY_TYPE::BASIC, glm::vec3(400.0f, 500.0f, 0.0f), 2, tracker++);
	//AddEntity(3, 3, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 500.0f, 0.0f), 3, tracker++);
	//AddEntity(3, 4, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 500.0f, 0.0f), 4, tracker++);

	//AddEntity(4, 1, ENTITY_TYPE::BASIC, glm::vec3(300.0f, 600.0f, 0.0f), 1, tracker++);
	//AddEntity(4, 2, ENTITY_TYPE::BASIC, glm::vec3(400.0f, 600.0f, 0.0f), 2, tracker++);
	//AddEntity(4, 3, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 600.0f, 0.0f), 3, tracker++);
	//AddEntity(4, 4, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 600.0f, 0.0f), 4, tracker++);

	//AddEntity(5, 1, ENTITY_TYPE::BASIC, glm::vec3(300.0f, 700.0f, 0.0f), 1, tracker++);
	//AddEntity(5, 2, ENTITY_TYPE::BASIC, glm::vec3(400.0f, 700.0f, 0.0f), 2, tracker++);
	//AddEntity(5, 3, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 700.0f, 0.0f), 3, tracker++);
	//AddEntity(5, 4, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 700.0f, 0.0f), 4, tracker++);
}

