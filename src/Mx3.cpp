#include "Mx3.hpp"
#include "fmod_errors.h"
#include <iostream>

// Component Headers
#include "LoopRegion.hpp"

Mx3::Mx3(int maxChannels, FMOD_INITFLAGS flags, void *externalDriverData) :
	mSystem(nullptr),
	mChannel(nullptr),
	result(FMOD_OK),
	version(0)
{
	result = FMOD::System_Create(&mSystem);
	ErrorCheck(result);

	result = mSystem->getVersion(&version);
	ErrorCheck(result);

	if(version < FMOD_VERSION)
		std::cerr << "FMOD library version " << version << " doesn't match header version " << FMOD_VERSION << std::endl;

	result = mSystem->init(maxChannels, flags, externalDriverData);
	ErrorCheck(result);
}

Mx3::~Mx3()
{
	for each(FMOD::Sound* sound in mSounds)
	{
		result = sound->release();
		ErrorCheck(result);
	}

	mSounds.clear();

	result = mSystem->close();
	ErrorCheck(result);

	result = mSystem->release();
	ErrorCheck(result);
}

void Mx3::play(std::string filepath)
{
	bool isActive = false;
	
	if(mChannel)
	{
		result = mChannel->isPlaying(&isActive);
		if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
			ErrorCheck(result);
		else if(isActive)
			stop();

		result = mChannel->getPaused(&isActive);
		if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
			ErrorCheck(result);
		else if(isActive)
			stop();
	}

	std::string ext = filepath.substr(filepath.find_last_of('.') + 1);

	if(ext.compare("multi") == 0)
	{
		// Multi Song
	}
	else
	{
		FMOD::Sound *sound;
		result = mSystem->createStream(filepath.c_str(), FMOD_2D | FMOD_ACCURATETIME, 0, &sound);
		ErrorCheck(result);

		result = sound->getLength(&mLength, FMOD_TIMEUNIT_MS);
		ErrorCheck(result);

		result = mSystem->playSound(sound, 0, false, &mChannel);
		ErrorCheck(result);
		mSounds.push_back(sound);
	}
}

bool Mx3::isPlaying()
{
    
}

void Mx3::pause()
{
	bool paused;

	result = mChannel->getPaused(&paused);
	ErrorCheck(result);

	result = mChannel->setPaused(!paused);
	ErrorCheck(result);
}

unsigned int Mx3::getPosition()
{
	unsigned int ms = 0;

	result = mChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
	ErrorCheck(result);

	return ms;
}

void Mx3::changeTimePosition(unsigned int position)
{
	result = mChannel->setPosition(position, FMOD_TIMEUNIT_MS);
	ErrorCheck(result);
}

/*
void Mx3::resume()
{
    
}
*/

void Mx3::stop()
{
	result = mChannel->stop();
	ErrorCheck(result);

	for each(FMOD::Sound *sound in mSounds)
	{
		result = sound->release();
		ErrorCheck(result);
	}

	mSounds.clear();
	mChannel = nullptr;
}

unsigned int Mx3::getLength()
{
    return mLength;
}

void Mx3::setGlobalVolume(float value)
{
	result = mChannel->setVolume(value);
	ErrorCheck(result);
}

void Mx3::stopOne(int index)
{
    
}

void Mx3::ErrorCheck(FMOD_RESULT result, std::string header)
{
	if (result != FMOD_OK)
		std::cerr << header << "\n" << FMOD_ErrorString(result) << std::endl;
}