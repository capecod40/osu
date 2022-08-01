#pragma once

#include <glm/glm.hpp>
#include "Game.h"


class BeatMap
{
private:

	Game* game;

	const double BPM;
	const int BEATS_PER_BAR;

	int entity_tracker;

	void AddEntity(const int bar, const float beat, ENTITY_TYPE type, glm::vec3 center, int index, int tracker);

	void CurrentBeat(int& bar, double& beat) const;

public:
	BeatMap(Game* game, const double = 60, const int beatsPerBar = 4);

	void Map();
	
};


