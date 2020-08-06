#pragma once
#include <queue>
#include "link.hpp"
/*
Handles the simulation timesteps
*/
class EventQueue {
public:
	void AddEventTimeStep(int, Link&);
	std::pair<Link&, int> GetNextTimeStep();
	void ClearQueue();
private:
	std::priority_queue<std::pair<int, Link&>> _timeStepQueue;
};