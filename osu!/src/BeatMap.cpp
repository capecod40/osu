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
	int currentBar;
	double currentBeat;
	CurrentBeat(currentBar, currentBeat);

	if (entity_tracker == tracker)
	{
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
	bar = netBeats / BEATS_PER_BAR;
	beat = netBeats - bar * BEATS_PER_BAR + 1;
}

void BeatMap::Map() // sliders cannot be perfectly vertical; offset ending x coords by at least one
{
	int tracker = 0;

	int currentBar;
	double currentBeat;
	CurrentBeat(currentBar, currentBeat);

	if (currentBar == 47)
	{
		game->inMenu = true;
		delete this;
		return;
	}
 
	AddEntity(tracker++, 1, 1, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 400.0f, 0.0f), 1);
	AddEntity(tracker++, 1, 2, ENTITY_TYPE::BASIC, glm::vec3(900.0f, 500.0f, 0.0f), 2);
	AddEntity(tracker++, 1, 3, ENTITY_TYPE::BASIC, glm::vec3(1000.0f, 400.0f, 0.0f), 3);
	AddEntity(tracker++, 1, 4, ENTITY_TYPE::BASIC, glm::vec3(1100.0f, 500.0f, 0.0f), 4);

	AddEntity(tracker++, 2, 1, ENTITY_TYPE::SLIDER, glm::vec3(1300.0f, 600.0f, 0.0f), 1, glm::vec3(1301.0f, 700.0f, 0.0f), 2);

	AddEntity(tracker++, 3, 1, ENTITY_TYPE::BASIC, glm::vec3(1100.0f, 800.0f, 0.0f), 1);
	AddEntity(tracker++, 3, 2, ENTITY_TYPE::BASIC, glm::vec3(1000.0f, 600.0f, 0.0f), 2);
	AddEntity(tracker++, 3, 3, ENTITY_TYPE::BASIC, glm::vec3(900.0f, 800.0f, 0.0f), 3);
	AddEntity(tracker++, 3, 4, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 600.0f, 0.0f), 4);

	AddEntity(tracker++, 4, 1, ENTITY_TYPE::SLIDER, glm::vec3(800.0f, 400.0f, 0.0f), 1, glm::vec3(801.0f, 300.0f, 0.0f), 2);

	// First things first
	AddEntity(tracker++, 5, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 5, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);

	// (Ima) say all the things
	AddEntity(tracker++, 6, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(70.0f, 70.0f), 0.0f), 0);

	AddEntity(tracker++, 6, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(70.0f, 70.0f), 0.0f), 2);
	AddEntity(tracker++, 6, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(70.0f, 70.0f), 0.0f), 3);

	// (Inside my) head I'm fired up
	AddEntity(tracker++, 7, 1, ENTITY_TYPE::BASIC, glm::vec3(1400.0f, 800.0f, 0.0f), 1);
	AddEntity(tracker++, 7, 2, ENTITY_TYPE::BASIC, glm::vec3(1200.0f, 800.0f, 0.0f), 2);
	AddEntity(tracker++, 7, 3, ENTITY_TYPE::SLIDER, glm::vec3(1000.0f, 800.0f, 0.0f), 3, glm::vec3(1050.0f, 900.0f, 0.0f), 0);

	AddEntity(tracker++, 8, 2.75, ENTITY_TYPE::BASIC, glm::vec3(1400.0f, 450.0f, 0.0f), 1);
	AddEntity(tracker++, 8, 3, ENTITY_TYPE::BASIC, glm::vec3(1400.0f, 300.0f, 0.0f), 2);
	AddEntity(tracker++, 8, 3.75, ENTITY_TYPE::BASIC, glm::vec3(1100.0f, 450.0f, 0.0f), 3);
	AddEntity(tracker++, 8, 4, ENTITY_TYPE::BASIC, glm::vec3(1100.0f, 300.0f, 0.0f), 4);

	AddEntity(tracker++, 9, 1, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 300.0f, 0.0f), 1);
	AddEntity(tracker++, 9, 2, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 500.0f, 0.0f), 2);

	AddEntity(tracker++, 9, 3, ENTITY_TYPE::SLIDER, glm::vec3(1000.0f, 600.0f, 0.0f), 3, glm::vec3(900.0f, 500.0f, 0.0f), 1);

	// The way that things have (2nd)
	AddEntity(tracker++, 10, 2.75, ENTITY_TYPE::BASIC, glm::vec3(1400.0f, 450.0f, 0.0f), 1);
	AddEntity(tracker++, 10, 3, ENTITY_TYPE::BASIC, glm::vec3(1400.0f, 300.0f, 0.0f), 2);
	AddEntity(tracker++, 10, 3.75, ENTITY_TYPE::BASIC, glm::vec3(1100.0f, 450.0f, 0.0f), 3);
	AddEntity(tracker++, 10, 4, ENTITY_TYPE::BASIC, glm::vec3(1100.0f, 300.0f, 0.0f), 4);
	AddEntity(tracker++, 10, 4.75, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 450.0f, 0.0f), 1);

	AddEntity(tracker++, 11, 1, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 300.0f, 0.0f), 2);
	AddEntity(tracker++, 11, 2, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 700.0f, 0.0f), 1);
	AddEntity(tracker++, 11, 3, ENTITY_TYPE::BASIC, glm::vec3(1000.0f, 700.0f, 0.0f), 2);
	AddEntity(tracker++, 11, 4, ENTITY_TYPE::BASIC, glm::vec3(1200.0f, 700.0f, 0.0f), 3);
	AddEntity(tracker++, 12, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 4);


	// Second things second
	AddEntity(tracker++, 13, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(40.0f, 40.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(-40.0f, 40.0f), 0.0f), 0);
	AddEntity(tracker++, 13, 2, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-40.0f, 40.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(-41.0f, -40.0f), 0.0f), 0);
	AddEntity(tracker++, 13, 3, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-40.0f, -40.0f), 0.0f), 3, glm::vec3(SCREEN_CENTER(40.0f, -40.0f), 0.0f), 0);
	AddEntity(tracker++, 13, 4, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(40.0f, -40.0f), 0.0f), 4, glm::vec3(SCREEN_CENTER(41.0f, 40.0f), 0.0f), 0);

	AddEntity(tracker++, 14, 2, ENTITY_TYPE::BASIC, glm::vec3(1200.0f, 350.0f, 0.0f), 1);
	AddEntity(tracker++, 14, 3, ENTITY_TYPE::BASIC, glm::vec3(1000.0f, 350.0f, 0.0f), 2);
	AddEntity(tracker++, 14, 4, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 350.0f, 0.0f), 3);
	AddEntity(tracker++, 15, 1, ENTITY_TYPE::BASIC, glm::vec3(600.0f, 350.0f, 0.0f), 4);

	AddEntity(tracker++, 15, 2, ENTITY_TYPE::SLIDER, glm::vec3(700.0f, 200.0f, 0.0f), 1, glm::vec3(725.0f, 225.0f, 0.0f), 0);
	AddEntity(tracker++, 15, 3, ENTITY_TYPE::SLIDER, glm::vec3(900.0f, 200.0f, 0.0f), 2, glm::vec3(925.0f, 225.0f, 0.0f), 0);
	AddEntity(tracker++, 15, 4, ENTITY_TYPE::SLIDER, glm::vec3(1100.0f, 200.0f, 0.0f), 3, glm::vec3(1125.0f, 225.0f, 0.0f), 0);

	//AddEntity(tracker++, 16, 1, ENTITY_TYPE::BASIC, glm::vec3(1125.0f, 225.0f, 0.0f), 4);
	AddEntity(tracker++, 16, 2, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 500.0f, 0.0f), 1);
	AddEntity(tracker++, 16, 3, ENTITY_TYPE::BASIC, glm::vec3(1125.0f, 500.0f, 0.0f), 2);
	AddEntity(tracker++, 16, 4, ENTITY_TYPE::BASIC, glm::vec3(800.0f, 800.0f, 0.0f), 3);

	AddEntity(tracker++, 17, 1, ENTITY_TYPE::BASIC, glm::vec3(1125.0f, 800.0f, 0.0f), 4);
	AddEntity(tracker++, 17, 2, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 17, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);

	AddEntity(tracker++, 18, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-500.0f, 100.0f), 0.0f), 1);
	AddEntity(tracker++, 18, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-200.0f, 300.0f), 0.0f), 2);
	AddEntity(tracker++, 19, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(200.0f, 300.0f), 0.0f), 3);
	AddEntity(tracker++, 19, 2, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(500.0f, 100.0f), 0.0f), 4);

	AddEntity(tracker++, 19, 3, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(100.0f, 0.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(-100.0f, 0.0f), 0.0f), 1);

	// I was broken
	AddEntity(tracker++, 21, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-100.0f, 50.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(100.0f, -50.0f), 0.0f), 0);
	AddEntity(tracker++, 22, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(100.0f, 50.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(-100.0f, -50.0f), 0.0f), 0);
	AddEntity(tracker++, 23, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(50.0f, 0.0f), 0.0f), 3, glm::vec3(SCREEN_CENTER(-50.0f, 0.0f), 0.0f), 1);

	// Took to me, shook to me
	AddEntity(tracker++, 24, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(200.0f, 200.0f), 0.0f), 1);
	AddEntity(tracker++, 24, 2, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-200.0f, 200.0f), 0.0f), 2);
	AddEntity(tracker++, 24, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-200.0f, -200.0f), 0.0f), 3);
	AddEntity(tracker++, 24, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(200.0f, -200.0f), 0.0f), 4);

	// Heartache
	AddEntity(tracker++, 25, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 25, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);

	AddEntity(tracker++, 26, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-250.0f, 150.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(150.0f, -100.0f), 0.0f), 0);
	AddEntity(tracker++, 28, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(250.0f, 150.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(-150.0f, -100.0f), 0.0f), 0);

	// broken?

	// Pain
	AddEntity(tracker++, 29, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 30, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(150.0f, 150.0f), 0.0f), 1);
	AddEntity(tracker++, 30, 2, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(150.0f, 150.0f), 0.0f), 2);
	AddEntity(tracker++, 30, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-150.0f, 150.0f), 0.0f), 3);
	AddEntity(tracker++, 30, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-150.0f, 150.0f), 0.0f), 4);

	// (Be)liever
	AddEntity(tracker++, 31, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-40.0f, 0.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(40.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 32, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(0.0f, 40.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(1.0f, -40.0f), 0.0f), 1);

	// Pain (2nd)
	AddEntity(tracker++, 33, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-100.0f, 100.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 0);
	AddEntity(tracker++, 33, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);

	// Break me down, you build me up
	AddEntity(tracker++, 34, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(100.0f, 100.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 0);
	AddEntity(tracker++, 34, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);

	// (Be)liever
	AddEntity(tracker++, 35, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-40.0f, -200.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(40.0f, -200.0f), 0.0f), 1);
	AddEntity(tracker++, 36, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);
	AddEntity(tracker++, 36, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 36, 4.25, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);
	AddEntity(tracker++, 36, 4.5, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 3);
	AddEntity(tracker++, 36, 4.75, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 4);

	// Pain
	AddEntity(tracker++, 37, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);

	// Let the bullets fly
	AddEntity(tracker++, 38, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-200.0f, -150.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(-201.0f, -180.0f), 0.0f), 3);
	AddEntity(tracker++, 38, 3, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-100.0f, -150.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(-101.0f, -180.0f), 0.0f), 3);

	// Rain
	AddEntity(tracker++, 39, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-350.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 39, 1.5, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-250.0f, 50.0f), 0.0f), 2);
	AddEntity(tracker++, 39, 2, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-150.0f, 100.0f), 0.0f), 3);
	AddEntity(tracker++, 39, 2.5, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(-50.0f, 150.0f), 0.0f), 4);
	AddEntity(tracker++, 39, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(50.0f, 150.0f), 0.0f), 1);
	AddEntity(tracker++, 39, 3.5, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(150.0f, 100.0f), 0.0f), 2);
	AddEntity(tracker++, 39, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(250.0f, 50.0f), 0.0f), 3);
	AddEntity(tracker++, 39, 4.5, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(350.0f, 0.0f), 0.0f), 4);

	// (My) life, my love, my drive
	AddEntity(tracker++, 40, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(200.0f, -150.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(201.0f, -180.0f), 0.0f), 3);
	AddEntity(tracker++, 40, 3, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(100.0f, -150.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(101.0f, -180.0f), 0.0f), 3);
	
	// Pain
	AddEntity(tracker++, 41, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);

	AddEntity(tracker++, 42, 1, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 42, 1.33, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);
	AddEntity(tracker++, 42, 1.66, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 3);
	AddEntity(tracker++, 42, 2, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 4);
	AddEntity(tracker++, 42, 3, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 1);
	AddEntity(tracker++, 42, 3.33, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 2);
	AddEntity(tracker++, 42, 3.66, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 3);
	AddEntity(tracker++, 42, 4, ENTITY_TYPE::BASIC, glm::vec3(SCREEN_CENTER(0.0f, 0.0f), 0.0f), 4);

	AddEntity(tracker++, 43, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(-150.0f, 150.0f), 0.0f), 1, glm::vec3(SCREEN_CENTER(100.0f, -125.0f), 0.0f), 0);
	AddEntity(tracker++, 44, 1, ENTITY_TYPE::SLIDER, glm::vec3(SCREEN_CENTER(150.0f, 150.0f), 0.0f), 2, glm::vec3(SCREEN_CENTER(-100.0f, -125.0f), 0.0f), 0);
}

