#pragma once

#include <SFML/Graphics.hpp>

class KeyViewerSystem final : public sf::Drawable, public sf::Transformable
{
public:
	struct Key
	{
		sf::Keyboard::Scan scan{};
		sf::Vector2u pos;
		sf::Color releasedColor{ 255, 255, 255, 0 };
		sf::Color pressedColor{ 255, 255, 255, 255 };
		sf::Color rainColor{ 255, 255, 255, 255 };
	};
	struct Stamp
	{
		bool press{};
		sf::Time time;
	};

	KeyViewerSystem();
	std::vector<Key> getKeys();
	void setKeys(const std::vector<Key>& keys);
	void setKeyLimiter(const std::vector<sf::Keyboard::Scan>& keyLimiter);
    void setKeyLimiterAuto(const std::vector<sf::Keyboard::Scan>& keyLimiter);
	void press(sf::Keyboard::Scan scan);
	void release(sf::Keyboard::Scan scan);
	void setReleasedColor(sf::Color releasedColor);
	void setPressedColor(sf::Color pressedColor);
	void setRainColor(sf::Color rainColor);
	void setRainColorByRow(sf::Color rainColor, unsigned int row);
	void update();

	Key& operator[] (size_t index);
	const Key& operator[] (size_t index) const;

private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	std::vector<Key> m_keys;
	std::unordered_map<sf::Keyboard::Scan, std::vector<Stamp>> m_keyPressed;
	sf::Clock m_clock;
	sf::Time m_rainSpeed;
	float m_rainLength;
	float m_keySize;
	float m_gapSize;
	float m_rainKeyGapSize;
};