#include "Mx3.hpp"
#include "fmod_errors.h"
#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <system_error>
#include <chrono>
#include <map>

#ifdef WIN32
#include <combaseapi.h>
#endif // WIN32

// Component Headers
#include "LoopRegion.hpp"

using json = nlohmann::json;

Mx3::Mx3(int maxChannels, FMOD_INITFLAGS flags, void *externalDriverData) :
	mSystem(nullptr),
	mChannel(nullptr),
	result(FMOD_OK),
	version(0),
	mMutex()
{
#ifdef WIN32
	HRESULT r = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(r != S_OK)
		std::cerr << "Error: Failed to initialize COM" << std::endl;
#endif // WIN32

	result = FMOD::System_Create(&mSystem);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->getVersion(&version);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	if(version < FMOD_VERSION)
		std::cerr << "FMOD library version " << version << " doesn't match header version " << FMOD_VERSION << std::endl;

	result = mSystem->init(maxChannels, flags, externalDriverData);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	try
	{
		mUpdate = new std::thread(&Mx3::update, this);
		mUpdate->detach();
	}
	catch(std::system_error &e)
	{
		std::cerr << "System error thrown: (" << e.code() << ")\n" << e.what() << std::endl;
	}
}

Mx3::~Mx3()
{
	mMutex.lock();
	shouldQuit = true;
	mMutex.unlock();
	
	for each(FMOD::Sound* sound in mSounds)
	{
		result = sound->release();
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
	}

	mSounds.clear();

	result = mSystem->close();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->release();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

#ifdef WIN32
	CoUninitialize();
#endif // WIN32
}

void Mx3::update()
{
#ifdef WIN32
	HRESULT r = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(r != S_OK)
		std::cerr << "Error: Failed to initialize COM" << std::endl;
#endif // WIN32
	
	while(!shouldQuit)
	{
		for(int i = 0; i < mComponents.size(); i++)
			mComponents[i]->update();
		
		result = mSystem->update();
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

#ifdef WIN32
	CoUninitialize();
#endif // WIN32
}

void Mx3::play(std::string filepath)
{
	bool isActive = false;
	
	if(mChannel != nullptr)
	{
		result = mChannel->isPlaying(&isActive);
		if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
		else if(isActive)
			stop();

		result = mChannel->getPaused(&isActive);
		if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
		else if(isActive)
			stop();
	}

	std::string ext = filepath.substr(filepath.find_last_of('.') + 1);

	if(ext.compare("multi") == 0)
	{
		std::ifstream file;
		json doc;

		file.open(filepath, std::ios::in);

		if(!file.is_open())
		{
			std::cerr << "Error: could not open " << filepath << std::endl;
			return;
		}

		file >> doc;
		file.close();
		
		std::vector<std::string> files = doc["files"];
		for each(std::string song in files)
		{
			FMOD::Sound *sound;
			result = mSystem->createStream(song.c_str(), FMOD_2D | FMOD_ACCURATETIME | FMOD_LOOP_NORMAL, 0, &sound);
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

			result = sound->getLength(&mLength, FMOD_TIMEUNIT_MS);
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

			result = mSystem->playSound(sound, 0, true, &mChannel);
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

			mSounds.push_back(sound);
		}

		if(doc.contains("nested_loops"))
		{
			std::vector<std::map<std::string, unsigned int>> loops = doc["nested_loops"];
			for each(auto loop in loops)
			{
				auto region = std::make_unique<LoopRegion>(loop["start"], loop["end"], mChannel);
				mComponents.push_back(std::move(region));
			}
		}

		for(int i = 0; i < mComponents.size(); i++)
			mComponents[i]->entry();

		result = mChannel->setPaused(false);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
	}
	else
	{
		FMOD::Sound *sound;
		result = mSystem->createStream(filepath.c_str(), FMOD_2D | FMOD_ACCURATETIME, 0, &sound);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		result = sound->getLength(&mLength, FMOD_TIMEUNIT_MS);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		result = mSystem->playSound(sound, 0, false, &mChannel);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
		mSounds.push_back(sound);
	}
}

bool Mx3::isPlaying()
{
	bool playing = false;
	result = mChannel->isPlaying(&playing);
	if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	return playing;
}

bool Mx3::isPaused()
{
	bool paused = false;
	result = mChannel->getPaused(&paused);
	if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	return paused;
}

void Mx3::pause()
{
	bool paused;

	result = mChannel->getPaused(&paused);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mChannel->setPaused(!paused);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
}

unsigned int Mx3::getPosition()
{
	unsigned int ms = 0;

	result = mChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	return ms;
}

void Mx3::changeTimePosition(unsigned int position)
{
	result = mChannel->setPosition(position, FMOD_TIMEUNIT_MS);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
}

/*
void Mx3::resume()
{
    
}
*/

void Mx3::stop()
{
	result = mChannel->stop();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	for each(FMOD::Sound *sound in mSounds)
	{
		result = sound->release();
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
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
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
}

void Mx3::stopOne(int index)
{
    
}

void Mx3::ErrorCheck(FMOD_RESULT result, std::string header)
{
	if(result != FMOD_OK)
		std::cerr << header << "\n" << FMOD_ErrorString(result) << std::endl;
}