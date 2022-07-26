#include "Song.hpp"
#include "fmod_errors.h"
#include <format>
#include <iostream>

#define ERRCHECK(x) ErrorCheck(x, __FILE__, __LINE__)
ErrorCallback Song::mErrorCallback = nullptr;

Song::Song(FMOD::System *system, FMOD::Sound *sound, FMOD::ChannelGroup *parent) :
mSystem(system),
mSound(sound),
mParent(parent),
eState(State::INIT),
mChannel(0)
{

   /* ERRCHECK(mSystem->createSound(file.c_str(),
        FMOD_2D | FMOD_NONBLOCKING | FMOD_ACCURATETIME | FMOD_LOOP_NORMAL | FMOD_CREATESTREAM,
        0,
        &mSound));*/
}

void Song::ErrorCheck(FMOD_RESULT result, const char* file, int line)
{
    if (result != FMOD_OK)
    {
        std::string msg = std::format("[Mx3]:\t{} ({}) - {}", file, line, FMOD_ErrorString(result));
        if (mErrorCallback)
            mErrorCallback(msg.c_str());
        else
            std::cerr << msg << std::endl;
    }
}

unsigned int Song::GetLength()
{
    return eState == State::PLAYING || eState == State::STOPPING ? Length : 0;
}

unsigned int Song::GetPosition()
{
    if (eState == State::PLAYING || eState == State::STOPPING)
    {
        unsigned int position;
        FMOD_RESULT result = mChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
        if (result == FMOD_ERR_CHANNEL_STOLEN || result == FMOD_ERR_INVALID_HANDLE)
        {
            eState = State::STOPPED;
            return 0;
        }
        ERRCHECK(result);

        return position;
    }

    return 0;
}

Song::State Song::GetState() const
{
    return eState;
}

bool Song::IsStopped() const
{
    return eState == State::STOPPED;
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
    FMOD_RESULT result = FMOD_OK;

    switch (mode)
    {
        case 0:
            result = mChannel->setMode(FMOD_LOOP_OFF);
            break;
        case 1:
        case 2:
            result = mChannel->setMode(FMOD_LOOP_NORMAL);
            break;
        default:
            break;
    }

    if (result == FMOD_ERR_CHANNEL_STOLEN || result == FMOD_ERR_INVALID_HANDLE)
    {
        eState = State::STOPPED;
        return;
    }

    ERRCHECK(result);
}

void Song::Update(float deltaTime)
{
    switch (eState)
    {
    case State::INIT:
        // One-time initialization (for now, just fall through)
    case State::TOPLAY:
    {
        if (bStopRequested)
        {
            eState = State::STOPPED;
            return;
        }

        FMOD_OPENSTATE state;
        ERRCHECK(mSound->getOpenState(&state, 0, 0, 0));
        if (state == FMOD_OPENSTATE_LOADING)
        {
            eState = State::LOADING;
            return;
        }

        ERRCHECK(mSystem->playSound(mSound, mParent, false, &mChannel));
        ERRCHECK(mChannel->setMode(FMOD_LOOP_NORMAL));
        ERRCHECK(mSound->getLength(&Length, FMOD_TIMEUNIT_MS));
        eState = State::PLAYING;
    }
        break;
    case State::LOADING:
    {
        FMOD_OPENSTATE openstate;
        ERRCHECK(mSound->getOpenState(&openstate, 0, 0, 0));

        if (openstate == FMOD_OPENSTATE_READY)
            eState = State::TOPLAY;
    }
        break;
    case State::PLAYING:
        if (bStopRequested)
        {
            // Going straight to STOPPED for now
            // Nothing for it yet
            eState = State::STOPPED;
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
    mChannel->stop();
}