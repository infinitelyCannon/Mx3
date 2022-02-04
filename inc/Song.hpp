#ifndef MX3_SONG
#define MX3_SONG

#include <functional>
#include "fmod.hpp"

typedef std::function<void(const char* msg, const char* file, int line)> ErrorCallback;

class Song 
{
public:
    Song(FMOD::System *system, FMOD::Sound *sound, FMOD::ChannelGroup *parent);
    ~Song();

    void Update(float deltaTime);
    bool IsStopped() const;
    static void SetErrorCallback(ErrorCallback callback);
    unsigned int GetPosition();
    unsigned int GetLength();
    void Stop();

private:
    void ErrorCheck(FMOD_RESULT reuslt, const char* file, int line);

    enum class State {INIT, TOPLAY, LOADING, PLAYING, STOPPING, STOPPED};
    State mState;
    FMOD::Channel *mChannel;
    FMOD::ChannelGroup *mParent;
    FMOD::Sound *mSound;
    FMOD::System *mSystem;
    bool bStopRequested = false;
    static ErrorCallback mErrorCallback;
};

#endif