#include "eventqueue.hpp"
#include <stdexcept>

// // This is needed to make priority_queue work with pairs
// bool operator<(const NWSim::Link&, const NWSim::Link&) {
// 	return true;
// }

// /*
// Efficiency of this will be O(log N * 2) for getting the next time step,
// where N is the amount of steps stored currently
// Adding timesteps will be O(log N)

// The values will be stored as negative, because priority_queue
// gives the max value by default and we want the lowest value
// (which is the greatest if made negative)
// */

// void EventQueue::ClearQueue() {
// 	// priority_queue doesn't have clear() method
// 	_timeStepQueue = std::priority_queue<std::pair<int, NWSim::Link&>>();
// }

// void EventQueue::AddEventTimeStep(int time, NWSim::Link& link) {
// 	_timeStepQueue.push({ -time, link });
// }

// std::pair<NWSim::Link&, int> EventQueue::GetNextTimeStep() {
// 	if (_timeStepQueue.empty()) {
// 		throw std::logic_error("Queue is empty");
// 	}
// 	auto p = _timeStepQueue.top();
// 	_timeStepQueue.pop();
// 	return {p.second, -p.first};
// }