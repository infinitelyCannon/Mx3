#include "LoopRegion.hpp"

LoopRegion::LoopRegion(unsigned int loopStart, unsigned int loopEnd, FMOD::Channel *channel) :
	mStart(loopStart),
	mEnd(loopEnd),
	mChannel(channel)
{

}

LoopRegion::~LoopRegion() 
{
	
}

FMOD_RESULT LoopRegion::entry()
{
	return mChannel->setLoopPoints(mStart, FMOD_TIMEUNIT_PCM, mEnd, FMOD_TIMEUNIT_PCM);
}

FMOD_RESULT LoopRegion::update()
{
	if (!shouldChange)
		return FMOD_OK;

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
{
	return FMOD_OK;
}