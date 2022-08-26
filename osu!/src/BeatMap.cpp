#include "BeatMap.h"

BeatMap::BeatMap(Game* game, const double bpm, const int beatsPerBar)
	:game(game), BPM(bpm), BEATS_PER_BAR(beatsPerBar), entity_tracker(0)
{
	glfwSetTime(0);
}

void BeatMap::AddEntity(int tracker,
	const int bar,
	const float beat,
	ENTITY_TYPE type,
	glm::vec3 startPos,
	int index /*= 0*/,
	glm::vec3 endPos /*= glm::vec3(0.0f)*/,
	int repeat /*= 0*/)
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
	double netMinutes = glfwGetTime() / 60.0; // for time delay, insert subtraction for sec / 60.0
	double netBeats = netMinutes * BPM;
	bar = netBeats / BEATS_PER_BAR;
	beat = netBeats - (bar) * BEATS_PER_BAR + 1;
}

void BeatMap::Map() // sliders cannot be perfectly vertical; offset ending x coords by at least one
{
	int tracker = 0;


	AddEntity(tracker++, 1, 1, ENTITY_TYPE::BASIC, glm::vec3(400.0f, 400.0f, 0.0f), 1);
	AddEntity(tracker++, 1, 2, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 500.0f, 0.0f), 2);
	AddEntity(tracker++, 1, 3, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 400.0f, 0.0f), 3);
	AddEntity(tracker++, 1, 4, ENTITY_TYPE::BASIC, glm::vec3(700.0f, 500.0f, 0.0f), 4);

	AddEntity(tracker++, 2, 1, ENTITY_TYPE::SLIDER, glm::vec3(600.0f, 600.0f, 0.0f), 1, glm::vec3(601.0f, 700.0f, 0.0f), 2);

	AddEntity(tracker++, 3, 1, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 800.0f, 0.0f), 1);
	AddEntity(tracker++, 3, 2, ENTITY_TYPE::BASIC, glm::vec3(700.0f, 600.0f, 0.0f), 2);
	AddEntity(tracker++, 3, 3, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 800.0f, 0.0f), 3);
	AddEntity(tracker++, 3, 4, ENTITY_TYPE::BASIC, glm::vec3(500.0f, 600.0f, 0.0f), 4);

	AddEntity(tracker++, 4, 1, ENTITY_TYPE::SLIDER, glm::vec3(800.0f, 700.0f, 0.0f), 1, glm::vec3(900.0f, 700.0f, 0.0f), 2);

	// First things first
	AddEntity(tracker++, 5, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER, 0.0f), 1);
	AddEntity(tracker++, 5, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER, 0.0f), 2);

	AddEntity(tracker++, 6, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER, 0.0f), 1, glm::vec3(SCREEN_CENTER_X + 70.0f, SCREEN_CENTER_Y + 70.0f, 0.0f), 0);

	AddEntity(tracker++, 6, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER_X + 70.0f, SCREEN_CENTER_Y + 70.0f, 0.0f), 2);
	AddEntity(tracker++, 6, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER, 0.0f), 3);

	// (Inside my) Head oh oooo
	AddEntity(tracker++, 7, 1, ENTITY_TYPE::BASIC, glm::vec3(1400.0f, 800.0f, 0.0f), 1);
	AddEntity(tracker++, 7, 1.33, ENTITY_TYPE::BASIC, glm::vec3(1350.0f, 800.0f, 0.0f), 2);
	AddEntity(tracker++, 7, 1.66, ENTITY_TYPE::BASIC, glm::vec3(1300.0f, 800.0f, 0.0f), 3);
						
	AddEntity(tracker++, 1, 1, ENTITY_TYPE::SLIDER, glm::vec3(1250.0f, 700.0f, 0.0f), 4, glm::vec3(1000.0f, 600.0f, 0.0f), 2);

}

