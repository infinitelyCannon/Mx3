#include "LoopRegion.hpp"

LoopRegion::LoopRegion(unsigned int loopStart, unsigned int loopEnd, std::vector<Track *> *tracks, int sampleRate) :
	mStart(loopStart),
	mEnd(loopEnd),
	sampleRate(sampleRate)
{
	trackRef = tracks;
}

LoopRegion::~LoopRegion() 
{
	
}

void LoopRegion::entry()
{
	for(Track *t : *trackRef)
	{
		for(TrackSource ts : *(t->getSourceRef()))
		{
			ErrorDelegate(
				ts.channel->setLoopPoints(mStart, FMOD_TIMEUNIT_MS, mEnd, FMOD_TIMEUNIT_MS),
				"Channel Loop Assignment:"
			);
		}
	}
}

void LoopRegion::update(std::vector<ComponentEvent> events)
{
	if (!shouldChange)
		return;

	shouldChange = false;
	
	for(Track *t : *trackRef)
	{
		for(TrackSource ts : *(t->getSourceRef()))
		{
			ErrorDelegate(
				ts.channel->setLoopPoints(mStart, FMOD_TIMEUNIT_MS, mEnd, FMOD_TIMEUNIT_MS),
				"Channel Loop Assignment:"
			);
		}
	}
}

void LoopRegion::change(unsigned int start, unsigned int end)
{
	shouldChange = true;
	mStart = start;
	mEnd = end;
}

void LoopRegion::exit()
{
	
}