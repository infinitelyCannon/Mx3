#ifndef MX3_SONG
#define MX3_SONG

#include <functional>
#include <string>
#include "fmod.hpp"

typedef std::function<void(const char* msg)> ErrorCallback;

class Song 
{
public:
    Song(FMOD::System *system, FMOD::Sound *sound, FMOD::ChannelGroup* parent);
    ~Song();

    enum class State { INIT, TOPLAY, LOADING, PLAYING, STOPPING, STOPPED };

    void Update(float deltaTime);
    bool IsStopped() const;
    State GetState() const;
    static void SetErrorCallback(ErrorCallback callback);
    unsigned int GetPosition();
    unsigned int GetLength();
    void Stop();
    void SetRepeatMode(int mode);

private:
    void ErrorCheck(FMOD_RESULT reuslt, const char* file, int line);
   
    State eState;
    FMOD::Channel *mChannel;
    FMOD::ChannelGroup *mParent;
    FMOD::Sound *mSound;
    FMOD::System *mSystem;
    unsigned int Length;
    bool bStopRequested = false;
    static ErrorCallback mErrorCallback;
};

#endif