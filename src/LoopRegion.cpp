#include "LoopRegion.hpp"
#include <iostream>

LoopRegion::LoopRegion(unsigned int loopStart, unsigned int loopEnd, FMOD::Channel *channel) :
	mStart(loopStart),
	mEnd(loopEnd),
	mChannel(channel)
{

}

LoopRegion::~LoopRegion() 
{
	
}

void LoopRegion::entry()
{
	result = mChannel->setLoopPoints(mStart, FMOD_TIMEUNIT_PCM, mEnd, FMOD_TIMEUNIT_PCM);
	ErrorCheck(result, "LoopRegion Line " + std::to_string(__LINE__ - 1));
}

void LoopRegion::update()
{
	if (!shouldChange)
		return;

	shouldChange = false;
	result = mChannel->setLoopPoints(mStart, FMOD_TIMEUNIT_PCM, mEnd, FMOD_TIMEUNIT_PCM);
	ErrorCheck(result, "LoopRegion Line " + std::to_string(__LINE__ - 1));
}

void LoopRegion::change(unsigned int start, unsigned int end)
{
	shouldChange = true;
	mStart = start;
	mEnd = end;
}

void LoopRegion::ErrorCheck(FMOD_RESULT result, std::string header)
{
	if(result != FMOD_OK)
		std::cerr << header << "\n" << FMOD_ErrorString(result) << std::endl;
}

void LoopRegion::exit()
{
	
}