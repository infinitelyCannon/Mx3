#include "Mx3.hpp"
#include "MultiSong.hpp"
#include <iostream>

Mx3::Mx3(int device, int frequency, uint32_t flags, void *win, void *clsid) : mChannels()
{
    BASS_Init(device, frequency, flags, win, clsid);
}

Mx3::~Mx3()
{
    delete songRef;
    BASS_Free();
    std::cout << "BASS Process Ended." << std::endl;
    ErrorCheck();
}

void CALLBACK Mx3::loopRegionTrigger(HSYNC handle, DWORD channel, DWORD data, void *user)
{
    for (DWORD chan : mChannels)
    {
        if (!BASS_ChannelSetPosition(chan, BASS_ChannelSeconds2Bytes(chan, *static_cast<double *>(user)), BASS_POS_BYTE))
            BASS_ChannelSetPosition(chan, 0, BASS_POS_BYTE);
    }
}

void Mx3::play(std::string filepath)
{
    if (mChannels.size() > 0)
        stop();

    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);

    if (ext.compare("multi") == 0)
    {
        songRef = new MultiSong(filepath);
        songRef->setup(&mChannels, &mLength, );

        for (DWORD channel : mChannels)
        {
            BASS_ChannelPlay(channel, TRUE);
            ErrorCheck("Mx3::play - line 41");
        }
    }
    else
    {
        DWORD channel;
        if (!(channel = BASS_StreamCreateFile(FALSE, filepath.c_str(), 0, 0, BASS_SAMPLE_LOOP)))
        {
            std::cout << "Cannot load file." << std::endl;
            ErrorCheck();
            return;
        }
        else
        {
            DWORD len = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            double tempLength = BASS_ChannelBytes2Seconds(channel, len);
            if (tempLength > mLength)
                mLength = tempLength;

            mChannels.push_back(channel);
            BASS_ChannelPlay(channel, TRUE);
            ErrorCheck();
        }
    }
}

bool Mx3::isPlaying()
{
    bool result = false;

    for (DWORD channel : mChannels)
    {
        if (BASS_ChannelIsActive(channel) != BASS_ACTIVE_STOPPED)
            result = result || true;
    }

    return result;
}

void Mx3::pause()
{
    for (DWORD channel : mChannels)
    {
        BASS_ChannelPause(channel);
        ErrorCheck();
    }
}

double Mx3::getPosition()
{
    if (mChannels.size() > 0)
    {
        double result = BASS_ChannelBytes2Seconds(mChannels[0], BASS_ChannelGetPosition(mChannels[0], BASS_POS_BYTE));
        ErrorCheck();
        return result;
    }
}

void Mx3::changeTimePosition(int position)
{
    for (DWORD channel : mChannels)
    {
        BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, position), BASS_POS_BYTE);
        ErrorCheck("Error changing position");
    }
}

void Mx3::resume()
{
    for (DWORD channel : mChannels)
    {
        BASS_ChannelPlay(channel, FALSE);
        ErrorCheck("Error while resuming");
    }
}

void Mx3::stop()
{
    for (DWORD channel : mChannels)
    {
        BASS_StreamFree(channel);
        ErrorCheck("Error while stopping channels");
    }

    mChannels.clear();
}

double Mx3::getLength()
{
    return mLength;
}

void Mx3::setGlobalVolume(float value)
{
    mVolume = value;

    for (DWORD channel : mChannels)
    {
        BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, mVolume);
        ErrorCheck("Error setting volume.");
    }
}

void Mx3::stopOne(HSTREAM handle)
{
    for (auto it = mChannels.begin(); it != mChannels.end(); it++)
    {
        if (*it == handle)
        {
            BASS_StreamFree(handle);
            ErrorCheck("Error while stopping channel");
            mChannels.erase(it);
            return;
        }
    }
}

void Mx3::stopOne(int index)
{
    if (index >= 0 && mChannels.size() > index)
    {
        auto it = mChannels.begin();
        it += index;
        BASS_StreamFree(*it);
        ErrorCheck();
        mChannels.erase(it);
    }
}

void Mx3::ErrorCheck(std::string header)
{
    int code = BASS_ErrorGetCode();

    if (code != BASS_OK)
        std::cout << header << "\nError Code: " << code << std::endl;
}