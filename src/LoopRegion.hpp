#ifndef LOOP_REGION_H
#define LOOP_REGION_H

#include "Component.hpp"
#include <string>

class LoopRegion : public Component
{
public:
	LoopRegion(unsigned int loopStart, unsigned int loopEnd, FMOD::Channel *channel);
	~LoopRegion();

	void entry() override;
	void update() override;
	void exit() override;

	void change(unsigned int start, unsigned int end);

private:
	void ErrorCheck(FMOD_RESULT result, std::string header = "");

	FMOD::Channel *mChannel;
	FMOD_RESULT result;
	unsigned int mStart;
	unsigned int mEnd;
	bool shouldChange = false;
};

#endif // !LOOP_REGION_H
