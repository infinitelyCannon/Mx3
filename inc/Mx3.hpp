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

class Mx3
{
public:
	Mx3();
	Mx3(int maxchannels, FMOD_INITFLAGS fmodFlags, void *driverdata = 0);
	~Mx3();

	void LoadSound(std::string file);
	void UnLoadSound(std::string file);
	int Play(std::string file); // Using Play instead of "PlaySound" because of Win macro
	void SetVolume(float volume, bool clamp = true);
	float GetVolume();
	void Update(float deltaTime);
	bool GetPaused();
	void SetPaused(bool paused);
	bool IsPlaying();
	unsigned int GetPosition();
	unsigned int GetLength();
	
	static void SetErrorCallback(ErrorCallback func);
	static FMOD_RESULT ChannelCallback(FMOD_CHANNELCONTROL* ctrl, FMOD_CHANNELCONTROL_TYPE ctrlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callType, void* data1, void* data2);

private:
	FMOD::System* mSystem;
	SoundMap mSounds;
	ChannelMap mChannels;
	FMOD::ChannelGroup* MasterChannel;
	std::vector<Recyclable> RecycleBin;
	int NextChannelID;

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