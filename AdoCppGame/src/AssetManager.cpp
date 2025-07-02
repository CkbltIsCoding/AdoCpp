#include "AssetManager.h"
#include <cassert>

std::map<std::string, sf::SoundBuffer> AssetManager::m_SoundBuffers;
std::map<std::string, sf::Font> AssetManager::m_Fonts;
AssetManager* AssetManager::sInstance = nullptr;

AssetManager::AssetManager()
{
	assert(sInstance == nullptr);
	sInstance = this;
}

sf::SoundBuffer& AssetManager::GetSoundBuffer(std::string const& filename)
{
	auto& soundBufferMap = sInstance->m_SoundBuffers;
	if (const auto pairFound = soundBufferMap.find(filename); pairFound != soundBufferMap.end())
		return pairFound->second;
	if (auto& soundBuffer = soundBufferMap[filename]; soundBuffer.loadFromFile("assets/sound/" + filename))
		return soundBuffer;
	throw std::runtime_error("SoundBuffer not found: " + filename);
}

sf::Font& AssetManager::GetFont(std::string const& filename)
{
	auto& fontMap = sInstance->m_Fonts;
	if (const auto pairFound = fontMap.find(filename); pairFound != fontMap.end())
		return pairFound->second;
	if (auto& font = fontMap[filename]; font.openFromFile("assets/font/" + filename))
		return font;
	throw std::runtime_error("Font not found: " + filename);
}
