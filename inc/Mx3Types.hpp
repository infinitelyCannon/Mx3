#pragma once

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<int, FMOD::Channel*> ChannelMap;
typedef std::map<std::string, FMOD::ChannelGroup*> TrackMap;
typedef std::function<void(const char* msg, const char* file, int line)> ErrorCallback;