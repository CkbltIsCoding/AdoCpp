#pragma once
#include <SFML/Audio.hpp>
#include <iostream>

inline std::filesystem::path addHitsound(std::filesystem::path path, const std::vector<AdoCpp::Tile>& tiles,
                                         float* progress = nullptr, bool hardcore = false)
{
    if (progress)
        *progress = -1;
    sf::SoundBuffer origSb{path}, newSb{}, hitSb{"assets/sound/hit.wav"};
    assert(hitSb.getChannelCount() == 2);
    auto samples = new int16_t[origSb.getSampleCount()];
    memcpy(samples, origSb.getSamples(), sizeof(int16_t) * origSb.getSampleCount());
    // static constexpr short MAX = 32767, MIN = -32768;
    // static constexpr float hitVolume = 0.75f / 1.f;
    // // ReSharper disable CppFunctionalStyleCast
    // for (size_t k = 0; k < seconds.size(); k++)
    // {
    //     float f = 1, val;
    //     for (size_t i = 0; i < hitSb.getSampleCount() / hitSb.getChannelCount(); i++)
    //     {
    //         for (size_t j = 0; j < origSb.getChannelCount(); j++)
    //         {
    //             const size_t idx = (size_t(seconds[k] * origSb.getSampleRate()) + i) * origSb.getChannelCount() + j;
    //             const size_t hitIdx = i * hitSb.getChannelCount() + j % hitSb.getChannelCount();
    //             if (idx >= origSb.getSampleCount())
    //                 break;
    //             if (hardcore)
    //                 samples[idx] += hitSb.getSamples()[hitIdx]; // NOLINT(*-narrowing-conversions)
    //             else
    //             {
    //                 val = (float(samples[idx] + hitSb.getSamples()[hitIdx]) * hitVolume) * f;
    //                 if (val > MAX)
    //                     f = MAX / val, val = MAX;
    //                 if (val < MIN)
    //                     f = MIN / val, val = MIN;
    //                 if (f < 1)
    //                     f += (1 - f) / 32;
    //                 samples[idx] = short(val);
    //             }
    //         }
    //     }
    //     if (progress)
    //         *progress = float(k) / float(seconds.size());
    // }
    // ReSharper disable CppFunctionalStyleCast
    for (size_t k = 1 /* tile[0].beat = -INF */; k < tiles.size(); k++)
    {
        const bool midspin = k != tiles.size() - 1 && tiles[k + 1].angle.deg() == 999;
        float hitVolume =
            (hardcore ? 4.0f : 2.0f) * float(midspin ? tiles[k].midspinHitsoundVolume : tiles[k].hitsoundVolume) / 100;

        for (size_t i = 0; i < hitSb.getSampleCount() / 4; i++)
        {
            for (size_t j = 0; j < origSb.getChannelCount(); j++)
            {
                const size_t idx =
                    (size_t(tiles[k].seconds * origSb.getSampleRate()) + i) * origSb.getChannelCount() + j;
                const size_t hitIdx = i * 4 + j % 4;
                if (idx >= origSb.getSampleCount())
                    break;
                if (hardcore)
                    samples[idx] += hitSb.getSamples()[hitIdx] * hitVolume; // NOLINT(*-narrowing-conversions)
                else
                {
                    const int s1 = samples[idx],
                              s2 = int(float(hitSb.getSamples()[hitIdx]) * hitVolume) /* * tiles[k].hitsoundVolume */;
                    const auto s3 = s1 + s2 - (s1 * s2 / (s1 < 0 && s2 < 0 ? -32767 : 32767));

                    samples[idx] = int16_t(std::max(-32768, std::min(32767, s3)));
                }
            }
        }
        if (progress)
            *progress = float(k) / float(tiles.size() - 1);
    }
    // ReSharper restore CppFunctionalStyleCast
    if (progress)
        *progress = 2;
    if (!newSb.loadFromSamples(samples, origSb.getSampleCount(), origSb.getChannelCount(), origSb.getSampleRate(),
                               origSb.getChannelMap()))
        throw std::runtime_error("Couldn't load sound");
    std::string ext = path.extension().string();
    if (ext == ".mp3")
        ext = ".wav";
    path.replace_extension().concat("-hitsound").concat(ext);
    if (!newSb.saveToFile(path))
        throw std::runtime_error("Couldn't save sound");
    return path;
}
