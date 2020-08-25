#pragma once

class Simulatable
{
public:
    void Simulate();
    // Advance time till next event can happen. 
    std::vector<int> AdvanceTime() 
    {
        for (int i = 0; i < evt_times.size(); i++)
        {
            evt_times[i]--;
            if (evt_times[i] < 0) evt_times[i] = -1;
        }
        return evt_times;
    }
    void SetSize(const int size) { 
        for (int i = 0; i < size; i++) evt_times.push_back(-1);
    }
    void SetEventTime(const int t, const size_t idx) { 
        if(idx < evt_times.size()) evt_times[idx] = (t < 0) ? -1 : t;
    }
    std::vector<int> GetEventTimes() const { return evt_times; }
private:
    // Every object may have different number of simulatable points, 
    // e.g. Router is connected to multiple links and passing data to each 
    // -> all of them require a separate timestamp. 
    // e.g. Link has outgoing and ingoing ways, both need a value
    // Hold indexes tied...
    std::vector<int> evt_times;
};