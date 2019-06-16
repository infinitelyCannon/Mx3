#ifndef MULTISONG_H
#define MULTISONG_H

#include "nlohmann/json.hpp"
#include "bass.h"
#include <vector>
#include <string>
#include <memory>
#include <utility>

class MultiSong
{
public:
    MultiSong(std::string filepath);
    ~MultiSong();
    void setup(std::vector<DWORD> *channels, double *length, SYNCPROC *callback);

private:
    std::vector<std::unique_ptr<std::pair<double, double>>> mLoopRegions;
    nlohmann::json mJson;
    void ErrorCheck(std::string heaader = "");
    std::vector<HSYNC> handles;
};

#endif //MULTISONG_H