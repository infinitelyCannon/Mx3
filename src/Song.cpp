#include "Song.hpp"
#include "fmod_errors.h"

#define ERRCHECK(x) ErrorCheck(x, __FILE__, __LINE__)
ErrorCallback Song::mErrorCallback = nullptr;

Song::Song(FMOD::System *system, std::string file, FMOD::ChannelGroup *parent) :
mSystem(system),
mSound(0),
mParent(parent),
mState(State::LOADING),
mChannel(0)
{
    ERRCHECK(mSystem->createSound(file.c_str(),
        FMOD_2D | FMOD_NONBLOCKING | FMOD_ACCURATETIME | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM,
        0,
        &mSound));
}

void Song::ErrorCheck(FMOD_RESULT result, const char* file, int line)
{
    if (result != FMOD_OK)
    {
        if (mErrorCallback)
            mErrorCallback(FMOD_ErrorString(result), file, line);
    }
}

unsigned int Song::GetLength()
{
    if (mState == State::PLAYING || mState == State::STOPPING)
    {
        unsigned int len = 0;
        FMOD_RESULT result = mSound->getLength(&len, FMOD_TIMEUNIT_MS);
        if (result == FMOD_ERR_CHANNEL_STOLEN || result == FMOD_ERR_INVALID_HANDLE)
        {
            mState = State::STOPPED;
            return 0;
        }
        ERRCHECK(result);

        return len;
    }
    
    return 0;
}

unsigned int Song::GetPosition()
{
    if (mState == State::PLAYING || mState == State::STOPPING)
    {
        unsigned int position;
        FMOD_RESULT result = mChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
        if (result == FMOD_ERR_CHANNEL_STOLEN || result == FMOD_ERR_INVALID_HANDLE)
        {
            mState = State::STOPPED;
            return 0;
        }
        ERRCHECK(result);

        return position;
    }

    return 0;
}

bool Song::IsStopped() const
{
    return mState == State::STOPPED;
}

void Song::Stop()
{
    bStopRequested = true;
}

void Song::SetErrorCallback(ErrorCallback callback)
{
    mErrorCallback = callback;
}

void Song::SetRepeatMode(int mode)
{
    switch (mode)
    {
        case 0:
            ERRCHECK(mChannel->setMode(FMOD_LOOP_OFF));
            break;
        case 1:
        case 2:
            ERRCHECK(mChannel->setMode(FMOD_LOOP_NORMAL));
            break;
        default:
            break;
    }
}

void Song::Update(float deltaTime)
{
    switch (mState)
    {
    case State::INIT:
        // One-time initialization (for now, just fall through)
    case State::TOPLAY:
        ERRCHECK(mSystem->playSound(mSound, mParent, false, &mChannel));
        ERRCHECK(mChannel->setMode(FMOD_LOOP_NORMAL));
        mState = State::PLAYING;
        break;
    case State::LOADING:
    {
        FMOD_OPENSTATE openstate;
        ERRCHECK(mSound->getOpenState(&openstate, 0, 0, 0));

        if (openstate == FMOD_OPENSTATE_READY)
            mState = State::INIT;
    }
        break;
    case State::PLAYING:
        if (bStopRequested)
        {
            // Going straight to STOPPED for now
            // Nothing for it yet
            mState = State::STOPPED;
            ERRCHECK(mChannel->stop());
        }
        break;
    case State::STOPPING:
        /* code */
        break;
    case State::STOPPED:
        /* code */
        break;   
    default:
        break;
    }
}

Song::~Song()
{
    FMOD_RESULT result;

    result = mChannel->stop();
    mSound->release();
    if (result != FMOD_OK && result != FMOD_ERR_INVALID_HANDLE)
        mErrorCallback(FMOD_ErrorString(result), __FILE__, __LINE__);
}