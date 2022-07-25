#ifndef MX3_MX3
#define MX3_MX3

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include "fmod.hpp"
#include "Song.hpp"

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<int, std::unique_ptr<Song>> ChannelMap;
//typedef std::map<std::string, FMOD::ChannelGroup*> TrackMap;
typedef std::pair<FMOD::Sound*, std::unique_ptr<Song>> Recyclable;
typedef std::function<void(const char* msg)> ErrorCallback;

struct Implementation
{
	Implementation() :
		mSystem(nullptr),
		NextChannelID(0)
	{};
	~Implementation();

	FMOD::System* mSystem;
	SoundMap Sounds;
	ChannelMap Channels;
	std::vector<Recyclable> Cleanup;

	int NextChannelID;
};

class Mx3
{
public:
	Mx3();
	Mx3(int maxchannels, FMOD_INITFLAGS flags, void *driverdata = 0);
	~Mx3();

	void LoadSound(std::string file);
	void UnLoadSound(std::string file);
	void PlaySound(std::string file);
	void SetVolume(float volume, bool clamp = true);
	float GetVolume();
	void Update(float deltaTime);
	void SetPaused(bool paused);
	bool IsPlaying();
	
	static void SetErrorCallback(ErrorCallback func);
	static FMOD_RESULT ChannelCallback(FMOD_CHANNELCONTROL* ctrl, FMOD_CHANNELCONTROL_TYPE ctrlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callType, void* data1, void* data2);

private:
	Implementation *mImplementation;

	void ErrorCheck(FMOD_RESULT result, const char *file, int line);
	bool IsSoundLoaded(const std::string file);

	static unsigned long long PCM2DSP(unsigned long long posiiton,
		unsigned long long soundRate,
		unsigned long long mixerRate);
	static double DSP2PCM(unsigned long long dsp, double soundRate, double mixerRate);
	static unsigned int PCM2MS(double pcm, float soundRate);

	static ErrorCallback mErrorCallback;
};

#endif