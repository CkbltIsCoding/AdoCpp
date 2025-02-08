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

	auto pairFound = soundBufferMap.find(filename);
	if (pairFound != soundBufferMap.end())
		return pairFound->second;

	auto& soundBuffer = soundBufferMap[filename];
	if (soundBuffer.loadFromFile("assets/sound/" + filename))
		return soundBuffer;
}

sf::Font& AssetManager::GetFont(std::string const& filename)
{
	auto& fontMap = sInstance->m_Fonts;
	
	auto pairFound = fontMap.find(filename);
	if (pairFound != fontMap.end())
		return pairFound->second;

	auto& font = fontMap[filename];
	if (font.openFromFile("assets/font/" + filename))
		return font;
}
