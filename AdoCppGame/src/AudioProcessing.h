#include <SFML/Audio.hpp>

std::filesystem::path addHitsound(std::filesystem::path path, std::vector<double> ms, float* progress = nullptr, bool hardcore = false)
{
	if (progress) *progress = -1;
	sf::SoundBuffer
		origSb{ path },
		newSb{},
		hitsoundSb{ "assets/sound/beat.wav" };
	assert(hitsoundSb.getChannelCount() == 1);
	auto samples = new int16_t[origSb.getSampleCount()];
	memcpy(
		samples,
		origSb.getSamples(),
		sizeof(int16_t) * origSb.getSampleCount()
	);
	static constexpr short MAX = 32767, MIN = -32768;
	static constexpr float hitsoundVolume = 0.75f / 1.f;
	for (size_t k = 0; k < ms.size(); k++)
	{
		float f = 1, val;
		for (size_t i = 0; i < hitsoundSb.getSampleCount(); i++)
		{
			const size_t index = (size_t(ms[k] / 1000 * origSb.getSampleRate()) + i)
				* origSb.getChannelCount();
			for (size_t j = 0; j < origSb.getChannelCount(); j++)
			{
				if (index + j >= origSb.getSampleCount()) break;
				if (hardcore)
					samples[index + j] += hitsoundSb.getSamples()[i];
				else
				{
					val = (samples[index + j] + hitsoundSb.getSamples()[i] * hitsoundVolume) * f;
					if (val > MAX) f = MAX / val, val = MAX;
					if (val < MIN) f = MIN / val, val = MIN;
					if (f < 1) f += (1 - f) / 32;
					samples[index + j] = (short)val;
				}
			}
		}
		if (progress) *progress = (float)k / ms.size();
	}
	if (progress) *progress = 2;
	if (!newSb.loadFromSamples(
		samples,
		origSb.getSampleCount(),
		origSb.getChannelCount(),
		origSb.getSampleRate(),
		origSb.getChannelMap()
	))
		throw std::exception();
	const auto ext = path.extension();
	path
		.replace_extension()
		.concat("-hitsound")
		.concat(ext.string());
	if (!newSb.saveToFile(path))
		throw std::exception();
	return path;
}