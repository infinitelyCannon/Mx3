#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include "fmod.hpp"

#ifdef WIN32
#undef PlaySound
#endif

typedef std::function<void(FMOD_RESULT, const char*, int)> ErrorCallback;
class Song;

struct Implementation
{
	Implementation();
	Implementation(int maxChannels, unsigned int flags, void* driverData);
	~Implementation();

	void Update(const float deltaTime = 0);
	bool HasSound(const std::string file);
	void LoadSound(const std::string file);
	void SetPaused(bool paused);
	bool GetPaused();
	void UnloadSound(const std::string file);
	void Stop();
	void SetVolume(float volume, const bool clamp = true);
	void SetChannelVolume(int channelID, float volume, bool clamp = true);

	FMOD::System* _system = nullptr;
	FMOD::ChannelGroup* _mainChannelGroup = nullptr;
	int _nextChannelID = 0;

	typedef std::map<std::string, std::vector<FMOD::Sound*>> SoundMap;
	typedef std::map<int, std::unique_ptr<Song>> ChannelMap;
	SoundMap _sounds;
	ChannelMap _channels;
};

class Mx3
{
public:
	Mx3();
	Mx3(int maxChannels, unsigned int flags, void* driverData);
	~Mx3();

	void Update(const float deltaTime);

	void PlaySound(const std::string path);
	//void StopChannel(int channelID);
	void SetPaused(bool paused);
	bool GetPaused();
	void Stop();
	void SetVolume(float volume, bool clamp = true);
	void SetChannelVolume(int channelID, float volume, bool clamp = true);
	static void SetErrorCallback(ErrorCallback callback);
	static ErrorCallback _errorCallback;

private:
	void LoadSound(const std::string path);
	void UnloadSound(const std::string path);

	Implementation* _implementation = nullptr;
	static Mx3* Instance; // For the FMOD callback(s)
};
