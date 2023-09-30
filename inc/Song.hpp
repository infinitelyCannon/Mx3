#pragma once

#include "fmod.hpp"
#include "Mx3.hpp"

class Song 
{
public:
    Song(Implementation& implementation, FMOD::Sound* sound);

    enum class State
    {
        Init,
        ToPlay,
        Loading,
        Playing,
        Stopping,
        Stopped
    };

    void Update(float deltaTime);
    bool IsPlaying();
    void SetVolume(float volume);
    inline State GetState() { return _state; }
    static ErrorCallback _errorCallback;

private:
    bool IsSoundLoaded();
    void ErrorCheck(FMOD_RESULT result, const char *file, int line);

    Implementation& _implementation;
    FMOD::Channel* _channel;
    FMOD::Sound* _sound;
    State _state;
};
