#ifndef MX3_MX3
#define MX3_MX3

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <utility>
#include "fmod.hpp"
#include "Song.hpp"

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<int, Song> ChannelMap;
typedef std::map<std::string, FMOD::ChannelGroup*> TrackMap;
typedef std::function<void(const char* msg, const char* file, int line)> ErrorCallback;

class Mx3
{
public:
	Mx3();
	Mx3(int maxchannels, FMOD_INITFLAGS flags, void *driverdata = 0);
	~Mx3();

	void Debug();

	void LoadSound(std::string file);
	void UnLoadSound(std::string file);
	void PlaySound(std::string file);
	void SetVolume(float volume, bool clamp = true);
	float GetVolume();
	void Update(float deltaTime);

	// Playlist Functionality
	void AddToQueue(std::string file);
	void AddToQueue(std::vector<std::string> &files);
	void RemoveFromQueue(int index);
	void ClearQueue();
	void Next();
	void Previous();
	void Play(int index = 0);
	void SetPaused(bool paused);
	void SwapSongs(int first, int second);
	bool GetPaused();
	void Stop();
	unsigned int GetPosition();
	unsigned int GetLength();
	void SetPosition(unsigned int position);
	bool IsPlaying();
	
	static void SetErrorCallback(ErrorCallback func);

private:
/*
	void LoadSound(const std::string name);
	void UnLoadSound(const std::string name);
	int PlaySound(const std::string name);
*/
	bool bDebug = false;
	void ErrorCheck(FMOD_RESULT result, const char *file, int line);
	bool IsSoundLoaded(const std::string file);

	static ErrorCallback mErrorCallback;
	std::vector<std::string> mQueue;
	SoundMap mSounds;
	ChannelMap mChannels;
	TrackMap mTracks;
	FMOD::System* mSystem;
	int NextChannelID = 0;
	int NowPlayingIdx = 0;
};

#endif