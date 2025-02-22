#include "KeyViewer.h"

KeyViewerSystem::KeyViewerSystem()
{
	m_clock.restart();
	m_rainSpeed = sf::seconds(0.4f);
	m_rainLength = 40.f;
	m_keySize = 10.f;
	m_gapSize = 0.5f;
	m_rainKeyGapSize = 0.5f;
}

std::vector<KeyViewerSystem::Key> KeyViewerSystem::getKeys()
{
	return m_keys;
}
void KeyViewerSystem::setKeys(std::vector<Key> keyLimiter)
{
	m_keys = keyLimiter;
	for (auto& key : keyLimiter)
	{
		m_keyPressed.try_emplace(key.scan, std::vector<Stamp>());
	}
}

void KeyViewerSystem::setKeyLimiter(std::vector<sf::Keyboard::Scan> keyLimiter)
{
	m_keys.clear();
	for (size_t i = 0; i < keyLimiter.size(); i++)
	{
		m_keys.push_back({ keyLimiter[i], sf::Vector2u(i, 0) });
		m_keyPressed.try_emplace(keyLimiter[i], std::vector<Stamp>());
	}
}

void KeyViewerSystem::press(sf::Keyboard::Scan scan)
{
	auto it = m_keyPressed.find(scan);
	if (it != m_keyPressed.end() && (it->second.empty() || !it->second.back().press))
		it->second.push_back(Stamp(true, m_clock.getElapsedTime()));
}

void KeyViewerSystem::release(sf::Keyboard::Scan scan)
{
	auto it = m_keyPressed.find(scan);
	if (it != m_keyPressed.end() && !it->second.empty() && it->second.back().press)
		it->second.push_back(Stamp(false, m_clock.getElapsedTime()));
}

void KeyViewerSystem::setReleasedColor(sf::Color releasedColor)
{
	for (auto& key : m_keys)
		key.releasedColor = releasedColor;
}

void KeyViewerSystem::setPressedColor(sf::Color pressedColor)
{
	for (auto& key : m_keys)
		key.pressedColor = pressedColor;
}

void KeyViewerSystem::setRainColor(sf::Color rainColor)
{
	for (auto& key : m_keys)
		key.rainColor = rainColor;
}

void KeyViewerSystem::setRainColorByRow(sf::Color rainColor, unsigned int row)
{
	for (auto& key : m_keys)
		if (key.pos.y == row)
			key.rainColor = rainColor;
}

void KeyViewerSystem::update()
{
}

KeyViewerSystem::Key& KeyViewerSystem::operator[](size_t index)
{
	return m_keys[index];
}

const KeyViewerSystem::Key& KeyViewerSystem::operator[](size_t index) const
{
	return m_keys[index];
}

void KeyViewerSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	static const sf::Color white = sf::Color::White,
		lightgray = sf::Color(200, 200, 200);
	states.transform *= getTransform();
	states.texture = nullptr;
	sf::RectangleShape sqrPrs{ { m_keySize, m_keySize } };
	sf::RectangleShape sqrRls{ { m_keySize, m_keySize } };
	const sf::Time time = m_clock.getElapsedTime();
	std::vector<Key> keyLimiter = m_keys;
	std::sort(keyLimiter.begin(), keyLimiter.end(),
		[](const Key& a, const Key& b) {return a.pos.y != b.pos.y ? a.pos.y < b.pos.y : a.pos.x < b.pos.x;});
	for (const auto& key : keyLimiter)
	{
		sqrPrs.setFillColor(key.pressedColor);
		sqrRls.setFillColor(key.releasedColor);

		const sf::Vector2f sqrPos{ key.pos.x * (m_keySize + m_gapSize), m_rainLength + m_rainKeyGapSize + key.pos.y * (m_keySize + m_gapSize) };
		std::vector<Stamp> stamps = m_keyPressed.at(key.scan);
		if (!stamps.empty() && stamps.back().press)
		{
			sqrPrs.setPosition(sqrPos);
			target.draw(sqrPrs, states);
		}
		else
		{
			sqrRls.setPosition(sqrPos);
			target.draw(sqrRls, states);
		}
		if (!stamps.empty() && stamps.back().press)
			stamps.push_back(Stamp(false, time));
		bool press = false;
		float begin = 0, end = 0;
		for (const auto& stamp : stamps)
		{
			press = stamp.press;
			if (press) begin = (stamp.time - time) / m_rainSpeed * m_rainLength + m_rainLength;
			else
			{
				end = (stamp.time - time) / m_rainSpeed * m_rainLength + m_rainLength;
				if (end <= 0) continue;
				if (begin < 0) begin = 0;
				sf::RectangleShape rect{ { m_keySize, end - begin } };
				rect.setFillColor(key.rainColor);
				rect.setPosition({ key.pos.x * (m_keySize + m_gapSize), begin });
				target.draw(rect, states);
			}
		}
	}
}
