#include "LoopRegion.hpp"

LoopRegion::LoopRegion(unsigned int loopStart, unsigned int loopEnd, FMOD::Channel *channel, FMOD_RESULT *result) :
	mStart(loopStart),
	mEnd(loopEnd),
	mChannel(channel)
{
	*result = mChannel->setLoopPoints(start, FMOD_TIMEUNIT_PCM, end, FMOD_TIMEUNIT_PCM);
}

LoopRegion::~LoopRegion() 
{
	
}

FMOD_RESULT LoopRegion::entry()
{}

FMOD_RESULT LoopRegion::update()
{
	if (!shouldChange)
		return;

	shouldChange = false;
	return mChannel->setLoopPoints(mStart, FMOD_TIMEUNIT_PCM, mEnd, FMOD_TIMEUNIT_PCM);
}

void LoopRegion::change(unsigned int start, unsigned int end)
{
	shouldChange = true;
	mStart = start;
	mEnd = end;
}

FMOD_RESULT LoopRegion::exit()
{}