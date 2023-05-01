#include "Mx3.hpp"
#include "fmod_errors.h"
#include "Song.hpp"
#include <iostream>
#include <format>
#include <algorithm>

/*
* TODO: Find a way to have the library be imported without the user having to use/include FMOD headers
* (Might help with glue implementations)
*/
Mx3::Mx3()
{
}

//double Mx3::DSP2PCM(unsigned long long dsp, double soundRate, double mixerRate)
//{
//	return dsp / mixerRate * soundRate;
//}

void Mx3::LoadSound(std::string file)
{

}

//unsigned long long Mx3::PCM2DSP(unsigned long long posiiton, unsigned long long soundRate, unsigned long long mixerRate)
//{
//	return posiiton / soundRate * mixerRate;
//}

//unsigned int Mx3::PCM2MS(double pcm, float soundRate)
//{
//	return (1000.0 * pcm) / soundRate;
//}

void Mx3::Update(float deltaTime)
{
	
}

Mx3::~Mx3()
{

}