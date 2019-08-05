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
	for(ComponentEvent e : events)
	{
		if(e.type.compare(LoopRegion::eventType()))
		{
			unsigned int *prevStart, *prevEnd, *start, *end;

			prevStart = static_cast<unsigned int *>(e.data1);
			prevEnd = static_cast<unsigned int *>(e.data2);
			start = static_cast<unsigned int *>(e.data3);
			end = static_cast<unsigned int *>(e.data4);

			if(*prevStart == mStart && *prevEnd == mEnd)
			{
				mStart = *start;
				mEnd = *end;
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