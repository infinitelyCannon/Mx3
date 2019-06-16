#ifndef MX3_H
#define MX3_H

#include "bass.h"
#include <vector>
#include <string>

class MultiSong;

class Mx3
{
public:
    AudioEngine(int device = -1, int frequency = 44100, uint32_t flags = 0, void *win = nullptr, void *clsid = nullptr);
    ~AudioEngine();
    bool isPlaying();
    void play(std::string filepath);
    void pause();
    void resume();
    void changeTimePosition(int position);
    void setGlobalVolume(float value);
    double getLength();
    void stop();
    double getPosition();

private:
    std::vector<DWORD> mChannels;
    void ErrorCheck(std::string header = "");
    void stopOne(HSTREAM handle);
    void stopOne(int index);
    float mVolume = 1.0f;
    double mLength = 0.0;
    MultiSong *songRef;
    void CALLBACK loopRegionTrigger(HSYNC handle, DWORD channel, DWORD data, void *user);
};

#endif //MX3_H