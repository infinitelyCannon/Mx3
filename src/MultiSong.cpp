#include "MultiSong.h"
#include <fstream>
#include <iostream>
#include <assert.h>

using json = nlohmann::json;

MultiSong::MultiSong(std::string filepath) : mLoopRegions(), mJson(), handles()
{
    std::ifstream file;
    file.open(filepath, std::ios::in);
    file >> mJson;
    file.close();
}

MultiSong::~MultiSong()
{
}

void MultiSong::setup(std::vector<DWORD> *channels, double *length, SYNCPROC *looPcallback)
{
    std::vector<std::string> files = mJson.value("files", std::vector<std::string>());

    assert(!files.empty());

    double tempLength = 0.0;
    for (std::string file : files)
    {
        DWORD channel;
        if (!(channel = BASS_StreamCreateFile(FALSE, file.c_str(), 0, 0, BASS_SAMPLE_LOOP)))
        {
            ErrorCheck("MutiSong cannot load file");
            return;
        }
        else
        {
            DWORD len = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            ErrorCheck();
            tempLength = BASS_ChannelGetLength(channel, len);
            ErrorCheck();

            if (tempLength > *length)
                *length = tempLength;

            channels->push_back(channel);
        }
    }

    std::vector<double> loopTimes = mJson.value("loop_regions", std::vector<double>(1, -1.0));

    if (loopTimes[0] != -1.0)
    {
        for (int i = 0; i < loopTimes.size(); i += 2)
        {
            auto loopPair = std::make_unique<std::pair<double, double>>();
            loopPair->first = loopTimes[i];
            loopPair->second = loopTimes[i + 1];
            for (auto it = channels->begin(); it != channels->end(); it++)
            {
                handles.push_back(BASS_ChannelSetSync(
                    *it,
                    BASS_SYNC_POS | BASS_SYNC_MIXTIME,
                    BASS_ChannelSeconds2Bytes(*it, loopPair->second),
                    looPcallback,
                    &loopPair->first));
                ErrorCheck("MultiSong::Setup - line 60.");
            }
        }
    }
}

/* QVariant MultiSong::toVariant() const
{
    QVariantMap map;

    QVariantList files;
    for (const auto &file : mFiles)
    {
        files.append(file);
    }
    map.insert("files", files);

    QVariantList nestLoops;
    for (const auto &loop : mLoopRegions)
    {
        nestLoops.append(loop->first);
        nestLoops.append(loop->second);
    }
    map.insert("LoopRegions", nestLoops);
}

void MultiSong::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    QVariantList files = map.value("files").toList();
    for (const QVariant &file : files)
    {
        mFiles.append(file.toString());
    }

    QVariantList loops = map.value("LoopRegions").toList();
    for (int i = 0; i < loops.size(); i += 2)
    {
        auto pair = std::make_unique<QPair<double, double>>();
        pair->first = loops[i].toDouble();
        pair->second = loops[i + 1].toDouble();
        mLoopRegions.push_back(std::move(pair));
    }
} */