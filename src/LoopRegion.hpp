#ifndef LOOP_REGION_H
#define LOOP_REGION_H

#include <string>

#include "Component.hpp"
#include "Track.hpp"

struct LoopRegionEvent : ComponentEvent
{
	unsigned int start;
	unsigned int end;
	unsigned int prevStart;
	unsigned int prevEnd;
};

class LoopRegion : public Component
{
public:
	LoopRegion(unsigned int loopStart, unsigned int loopEnd, std::vector<Track *> *tracks, int sampleRate);
	~LoopRegion();

	void entry() override;
	void update(std::vector<ComponentEvent> events) override;
	void exit() override;
	std::string getEventType() const override;

	void change(unsigned int start, unsigned int end);

private:
	int sampleRate = 0;
	std::vector<Track *> *trackRef;
	const std::string eventType = "LOOP_REGION";
	unsigned int mStart;
	unsigned int mEnd;
	bool shouldChange = false;
};

#endif // !LOOP_REGION_H
