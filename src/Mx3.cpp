#include "Mx3.hpp"
#include "fmod_errors.h"

#include <iostream>
#include <fstream>
#include <system_error>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>

#ifdef WIN32
#include <combaseapi.h>
#endif // WIN32

// Component Headers
#include "LoopRegion.hpp"
#include "Parameter.hpp"

using json = nlohmann::json;

Mx3::Mx3(int maxChannels, FMOD_INITFLAGS flags, void *externalDriverData) :
	mSystem(nullptr),
	mChannel(nullptr),
	result(FMOD_OK),
	mMutex(),
	mTracks()
{
#ifdef WIN32
	HRESULT r = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(r == S_FALSE)
		std::cerr << "Error: COM library already started on this thread." << std::endl;
	else if(r == RPC_E_CHANGED_MODE)
		std::cerr << "Error: COM library started under differnt concurrency model." << std::end;
	else if(r != S_OK)
		std::cerr << "Error: Failed to initialize COM" << std::endl;
#endif // WIN32
	FMOD::ChannelGroup *masterGroup;
	unsigned int version = 0;
	
	result = FMOD::System_Create(&mSystem);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->getVersion(&version);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	if(version < FMOD_VERSION)
		std::cerr << "FMOD library version " << version << " doesn't match header version " << FMOD_VERSION << std::endl;

	result = mSystem->init(maxChannels, flags, externalDriverData);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->createChannelGroup("Timeline", &mTimeline);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->getMasterChannelGroup(&masterGroup);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = masterGroup->addGroup(mTimeline);
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
	
	/*for(FMOD::Sound* sound : mSounds)
	{
		result = sound->release();
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
	}

	mSounds.clear();*/

	for(Track *t : mTracks)
		t->release();

	mTracks.clear();

	result = mTimeline->release();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->close();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mSystem->release();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

#ifdef WIN32
	CoUninitialize();
#endif // WIN32
}

float Mx3::A1(float x1, float x2, float x3, float y1, float y2, float y3)
{
	return ((x1 * (y2 - y3)) + (x2 * (y3 - y1)) + (x3 * (y1 - y2))) / 
		((2.0f * std::pow(x1 - x2, 2.0f)) * (x1 - x3) * (x2 - x3));
}

float Mx3::A2(float x1, float x2, float x3, float a1)
{
	return (x2 - x1) / (x2 - x3) * a1;
}

void Mx3::AddEvent(ComponentEvent eve)
{
	mEvents.push_back(eve);
}

float Mx3::B1(float x1, float x2, float x3, float y1, float y2, float y3)
{
	return (std::pow(x1, 2.0f) * (y3 - y2) - x1 * (x2 * (-3.0f * y1 + y2 + 2.0f * y3) + 3.0f * x3 * (y1 - y2)) + std::pow(x2, 2.0f) * (y3 - y1) + x2 *x3 * (y2 - y1) + 2.0f * std::pow(x3, 2.0f) * (y1 - y2))
		/ (2.0f * (x1 - x2) * (x1 - x3) * (x2 - x3));
}

float Mx3::B2(float x1, float x2, float x3, float y1, float y2, float y3)
{
	return (2.0f * std::pow(x1, 2.0f) * (y2 - y3) + x2 * (x1 * (y3 - y2) + x3 * (2.0f * y1 + y2 - 3.0f * y3)) + 3.0f * x1 * x3 * (y3 - y2) + std::pow(x2, 2.0f) * (y3 - y1) + std::pow(x3, 2.0f) * (y2 - y1))
		/ (2.0f * (x1 - x2) * (x1 - x3) * (x2 - x3));
}

void Mx3::BindErrorCallback(SA::delegate<void(std::string)> func)
{
	ErrorDelegate = func;
}

void Mx3::changeTimePosition(unsigned int position)
{
	result = mChannel->setPosition(position, FMOD_TIMEUNIT_MS);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
}

float Mx3::curveFunction(float x, Point start, Point mid, Point end)
{
	if(x >= start.x && x <= mid.x)
	{
		float a1 = A1(start.x, mid.x, end.x, start.y, mid.y, end.y);
		float b1 = B1(start.x, mid.x, end.x, start.y, mid.y, end.y);

		return (a1 * std::pow(x - start.x, 3.0f)) + (b1 * (x - start.x)) + start.y;
	}
	else if(x >= mid.x && x <= end.x)
	{
		float a1 = A1(start.x, mid.x, end.x, start.y, mid.y, end.y);
		float a2 = A2(start.x, mid.x, end.x, a1);
		float b2 = B2(start.x, mid.x, end.x, start.y, mid.y, end.y);

		return (a2 * std::pow(x - end.x, 3.0f)) + (b2 * (x - end.x)) + end.y;
	}
}

void Mx3::ErrorCheck(FMOD_RESULT result, std::string header)
{
	if(ErrorDelegate.isNull())
	{
		if(result != FMOD_OK)
			std::cerr << header << "\n" << FMOD_ErrorString(result) << std::endl;
	}
	else
	{
		if(result != FMOD_OK)
			ErrorDelegate("[" + header + "]: " + FMOD_ErrorString(result));
	}
}

unsigned int Mx3::getLength()
{
	if(mTracks.size() > 0)
		return mTracks[0]->mLength;
	else
		return 0;
}

unsigned int Mx3::getPosition()
{
	unsigned int ms = 0;

	if(!mTracks.empty())
	{
		result = mTracks[0]->mSources[0].channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
	}

	return ms;
}

bool Mx3::isPaused()
{
	bool paused = false;
	result = mTimeline->getPaused(&paused);
	if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	return paused;
}

bool Mx3::isPlaying()
{
	bool playing = false;
	result = mTimeline->isPlaying(&playing);
	if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	return playing;
}

void Mx3::parseNestedLoops(json &jdoc)
{
	std::vector<std::map<std::string, unsigned int>> loops = jdoc["nested_loops"];
	for(auto loop : loops)
	{
		LoopRegion *region = new LoopRegion(loop["start"], loop["end"], &mTracks, 0);
		region->setDelegate(SA::delegate<void(FMOD_RESULT, std::string)>::create<Mx3, &Mx3::ErrorCheck>(this));
		mComponents.push_back(region);
	}
}

void Mx3::parseParameters(json &jdoc)
{
	json::iterator it = jdoc["tracks"].begin();
	json::iterator end = jdoc["tracks"].end();
	int itCount = 0;

	for(; it != end; it++)
	{
		if(it->contains("params"))
		{
			json::iterator par = (*it)["params"].begin();
			json::iterator parEnd = (*it)["params"].end();
			for(; par != parEnd; par++)
			{
				Parameter *param = new Parameter(mTracks[itCount], (*par)["name"], (*par)["type"]);
				param->setDelegate(SA::delegate<void(FMOD_RESULT, std::string)>::create<Mx3, &Mx3::ErrorCheck>(this));
				mComponents.push_back(param);
			}
		}
		itCount++;
	}
}

void Mx3::pause()
{
	bool paused = true;

	result = mTimeline->getPaused(&paused);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	result = mTimeline->setPaused(!paused);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
}

void Mx3::play(std::string filepath)
{
	bool isActive = false;

	if(mTimeline != nullptr)
	{
		result = mTimeline->isPlaying(&isActive);
		if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
		else if(isActive)
			stop();

		result = mTimeline->getPaused(&isActive);
		if((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
		else if(isActive)
			stop();
	}
	else
	{
		FMOD::ChannelGroup *masterGroup;

		result = mSystem->createChannelGroup("Timeline", &mTimeline);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		result = mSystem->getMasterChannelGroup(&masterGroup);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		result = masterGroup->addGroup(mTimeline);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
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

		int numTracks = doc["tracks"].size();
		Track *track;

		result = mTimeline->setPaused(true);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		for(int i = 0; i < numTracks; i++)
		{
			FMOD::ChannelGroup *group;
			std::string groupName = doc["tracks"][i]["name"].get<std::string>();

			result = mSystem->createChannelGroup(groupName.c_str(), &group);
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

			result = mTimeline->addGroup(group);
			ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

			track = new Track(groupName, group);
			track->DeferError = SA::delegate<void(FMOD_RESULT, std::string)>::create<Mx3, &Mx3::ErrorCheck>(this);

			json::iterator it = doc["tracks"][i]["files"].begin();
			json::iterator end = doc["tracks"][i]["files"].end();
			for(; it != end; it++)
			{
				std::string s = *it;

				track->addSource(s, mSystem);
			}

			mTracks.push_back(track);
		}

		if(doc.contains("nested_loops"))
			parseNestedLoops(doc);

		if(doc.contains("parameters"))
			parseParameters(doc);

		for(Component *c : mComponents)
			c->entry();

		std::sort(mTracks.begin(), mTracks.end(), [](Track *a, Track *b)
		{
			return (*a) > (*b);
		});

		result = mTimeline->setPaused(false);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
	}
	else
	{
		FMOD::ChannelGroup *group = 0;

		result = mTimeline->setPaused(true);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		result = mSystem->createChannelGroup("Default", &group);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		result = mTimeline->addGroup(group);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		Track *track = new Track("Default", group);
		track->DeferError = SA::delegate<void(FMOD_RESULT, std::string)>::create<Mx3, &Mx3::ErrorCheck>(this);
		track->addSource(filepath, mSystem);

		mTracks.push_back(track);
		//Don't need to sort a size (1) vector here.

		result = mTimeline->setPaused(false);
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
	}
}

void Mx3::setGlobalVolume(float value)
{
	result = mChannel->setVolume(value);
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));
}

void Mx3::stop()
{
	result = mTimeline->stop();
	ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

	for(Track *t : mTracks)
		t->release();

	mTracks.clear();
	mTimeline = nullptr;
}

void Mx3::stopOne(int index)
{
    
}

void Mx3::update()
{
#ifdef WIN32
	HRESULT r = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if(r == S_FALSE)
		std::cerr << "Error: COM library already started on this thread." << std::endl;
	else if(r == RPC_E_CHANGED_MODE)
		std::cerr << "Error: COM library started under differnt concurrency model." << std::end;
	else if(r != S_OK)
		std::cerr << "Error: Failed to initialize COM" << std::endl;
#endif // WIN32

	while(!shouldQuit)
	{
		for(Component *c : mComponents)
			c->update(mEvents);

		if(!mEvents.empty())
			mEvents.clear();

		result = mSystem->update();
		ErrorCheck(result, "Mx3.cpp Line " + std::to_string(__LINE__ - 1));

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

#ifdef WIN32
	CoUninitialize();
#endif // WIN32
}