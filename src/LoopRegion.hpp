#ifndef LOOP_REGION_H
#define LOOP_REGION_H

#include "Component.hpp"

class LoopRegion : public Component
{
public:
	LoopRegion(unsigned int loopStart, unsigned int loopEnd, FMOD::Channel *channel);
	~LoopRegion();

	FMOD_RESULT entry() override;
	FMOD_RESULT update() override;
	FMOD_RESULT exit() override;

	void change(unsigned int start, unsigned int end);

private:
	FMOD::Channel *mChannel;
	unsigned int mStart;
	unsigned int mEnd;
	bool shouldChange = false;
};

#endif // !LOOP_REGION_H
