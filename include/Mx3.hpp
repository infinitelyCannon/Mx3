#ifndef MX3_H
#define MX3_H

#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include "fmod.hpp"
#include "Delegate.h"
#include "Track.hpp"
#include "Component.hpp"
#include "nlohmann/json.hpp"

struct Point
{
	float x;
	float y;
};

class Mx3
{
public:
	Mx3(int maxChannels, FMOD_INITFLAGS flags, void *externalDriverData);
	~Mx3();
	bool isPlaying();
	bool isPaused();
	void play(std::string filepath);
	void pause();
	void changeTimePosition(unsigned int position);
	void setGlobalVolume(float value);
	unsigned int getLength();
	void stop();
	unsigned int getPosition();
	void update();
	void BindErrorCallback(SA::delegate<void(std::string)> func);
	void AddEvent(ComponentEvent eve);

private:
	void ErrorCheck(FMOD_RESULT result, std::string header = "");
	void parseNestedLoops(nlohmann::json &jdoc);
	void parseParameters(nlohmann::json &jdoc);
	static float curveFunction(float x, Point start, Point mid, Point end);
	static float A1(float x1, float x2, float x3, float y1, float y2, float y3);
	static float A2(float x1, float x2, float x3, float a1);
	static float B1(float x1, float x2, float x3, float y1, float y2, float y3);
	static float B2(float x1, float x2, float x3, float y1, float y2, float y3);

	SA::delegate<void(std::string)> ErrorDelegate;
	bool shouldQuit = false;
	FMOD::System *mSystem;
	std::thread *mUpdate;
	std::mutex mMutex;
	FMOD::ChannelGroup *mTimeline = nullptr;
	FMOD::Channel *mChannel;
	FMOD_RESULT result;
	//std::vector<FMOD::Sound *> mSounds;
	std::vector<Component *> mComponents;
	std::vector<Track *> mTracks;
	std::vector<ComponentEvent> mEvents;
	void stopOne(int index);
	float mVolume = 1.0f;
	unsigned int mLength = 0;
};

#endif //MX3_H