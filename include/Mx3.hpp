#ifndef MX3_H
#define MX3_H

#include "fmod.hpp"
#include <string>
#include <vector>
#include <memory>

//#include "Component.hpp"
class Component;

class Mx3
{
public:
    Mx3(int maxChannels, FMOD_INITFLAGS flags, void *externalDriverData);
    ~Mx3();
    bool isPlaying();
    void play(std::string filepath);
    void pause();
    //void resume();
    void changeTimePosition(unsigned int position);
    void setGlobalVolume(float value);
    unsigned int getLength();
    void stop();
    unsigned int getPosition();

private:
    void ErrorCheck(FMOD_RESULT result, std::string header = "");

	FMOD::System *mSystem;
	FMOD::Channel *mChannel;
	FMOD_RESULT result;
	unsigned int version;
	std::vector<FMOD::Sound*> mSounds;
	std::vector<std::unique_ptr<Component>> mComponents;
    void stopOne(int index);
    float mVolume = 1.0f;
    unsigned int mLength = 0;
};

#endif //MX3_H