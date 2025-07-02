#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <map>
#include <SFML/Graphics/Font.hpp>

class AssetManager
{
public:
	AssetManager();

	static sf::SoundBuffer& GetSoundBuffer(std::string const& filename);
	static sf::Font& GetFont(std::string const& filename);

private:
	static std::map<std::string, sf::SoundBuffer> m_SoundBuffers;
	static std::map<std::string, sf::Font> m_Fonts;

	static AssetManager* sInstance;
};
