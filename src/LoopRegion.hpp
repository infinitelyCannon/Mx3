#ifndef LOOP_REGION_H
#define LOOP_REGION_H

#include <string>

#include "Component.hpp"
#include "Track.hpp"

class LoopRegion : public Component
{
public:
	LoopRegion(unsigned int loopStart, unsigned int loopEnd, std::vector<Track *> *tracks, int sampleRate);
	~LoopRegion();

	void entry() override;
	void update(std::vector<ComponentEvent> events) override;
	void exit() override;

	void change(unsigned int start, unsigned int end);

private:
	int sampleRate = 0;
	std::vector<Track *> *trackRef;
	unsigned int mStart;
	unsigned int mEnd;
	bool shouldChange = false;
};

#endif // !LOOP_REGION_H
