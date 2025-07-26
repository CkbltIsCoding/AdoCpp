#include "Tile.h"
#include <map>

TileShape::TileShape(const double l_lastAngle, const double l_angle, const double l_nextAngle)
{
    m_lastAngle = l_lastAngle, m_angle = l_angle, m_nextAngle = l_nextAngle;
    m_interpolationLevel = 16;
}
void TileShape::update()
{
    const double m_angle2 = m_angle == 999 ? m_lastAngle + 180 : m_angle;
    const sf::Angle angle = sf::degrees(static_cast<float>(m_angle2)),
                    nextAngle = sf::degrees(static_cast<float>(m_nextAngle));
    const double includedAngle = AdoCpp::includedAngle(m_angle2, m_nextAngle);
    if (m_nextAngle == 999)
    {
        std::vector<sf::Vector2f> vec = {{-0.25, 0.25}, {0, 0.25}, {0.25, 0}, {0, -0.25}, {-0.25, -0.25}};
        sf::Vector2f v(1, 0);
        v = v.rotatedBy(angle);
        for (auto& p : vec)
            p = p.rotatedBy(angle) + v;
        importVertexPositions(vec);
    }
    else if (includedAngle == 180)
    {
        std::vector<sf::Vector2f> vec = {{-0.5, 0.25}, {0.5, 0.25}, {0.5, -0.25}, {-0.5, -0.25}};
        for (auto& p : vec)
            p = p.rotatedBy(angle);
        importVertexPositions(vec);
    }
    else if (includedAngle == 360)
    {
        static constexpr float pi = 3.1415927f;
        std::vector<sf::Vector2f> vec;

        for (int i = 0; i <= m_interpolationLevel / 2; i++)
        {
            const float a = i * 2 * pi / m_interpolationLevel - angle.asRadians(), x = std::sin(a) * 0.25f,
                        y = std::cos(a) * 0.25f;

            vec.emplace_back(x, y);
        }
        vec.push_back(sf::Vector2f({-0.25, -0.25}).rotatedBy(angle));
        vec.push_back(sf::Vector2f({-0.25, 0.25}).rotatedBy(angle));
        importVertexPositions(vec);
    }
    else
    {
        sf::Vector2f a(-0.25, 0.25), b(0.25, 0.25), c(0.25, -0.25), d(-0.25, -0.25), e(-0.25, 0.25), f(0.25, 0.25),
            g(0.25, -0.25), h(-0.25, -0.25), v(-0.25, 0), w(0.25, 0), m, n;
        v = v.rotatedBy(angle), w = w.rotatedBy(nextAngle);
        a = a.rotatedBy(angle) + v, b = b.rotatedBy(angle) + v, c = c.rotatedBy(angle) + v, d = d.rotatedBy(angle) + v;
        e = e.rotatedBy(nextAngle) + w, f = f.rotatedBy(nextAngle) + w, g = g.rotatedBy(nextAngle) + w,
        h = h.rotatedBy(nextAngle) + w;
        if (60 <= includedAngle && includedAngle <= 300)
        {
            m.x = ((e.x - f.x) * (b.x * a.y - a.x * b.y) - (a.x - b.x) * (f.x * e.y - e.x * f.y)) /
                ((e.x - f.x) * (a.y - b.y) - (a.x - b.x) * (e.y - f.y));
            m.y = ((e.y - f.y) * (b.y * a.x - a.y * b.x) - (a.y - b.y) * (f.y * e.x - e.y * f.x)) /
                ((e.y - f.y) * (a.x - b.x) - (a.y - b.y) * (e.x - f.x));
            n.x = ((g.x - h.x) * (d.x * c.y - c.x * d.y) - (c.x - d.x) * (h.x * g.y - g.x * h.y)) /
                ((g.x - h.x) * (c.y - d.y) - (c.x - d.x) * (g.y - h.y));
            n.y = ((g.y - h.y) * (d.y * c.x - c.y * d.x) - (c.y - d.y) * (h.y * g.x - g.y * h.x)) /
                ((g.y - h.y) * (c.x - d.x) - (c.y - d.y) * (g.x - h.x));
            if (90 <= includedAngle && includedAngle <= 270)
                importVertexPositions({f, g, n, d, a, m});
            else if (includedAngle < 180)
                importVertexPositions({f, g, h, c, d, a, m});
            else
                importVertexPositions({n, d, a, b, e, f, g});
        }
        else
        {
            sf::Vector2f p;
            if (includedAngle < 180)
            {
                m.x = ((f.x - g.x) * (b.x * a.y - a.x * b.y) - (a.x - b.x) * (g.x * f.y - f.x * g.y)) /
                    ((f.x - g.x) * (a.y - b.y) - (a.x - b.x) * (f.y - g.y));
                m.y = ((f.y - g.y) * (b.y * a.x - a.y * b.x) - (a.y - b.y) * (g.y * f.x - f.y * g.x)) /
                    ((f.y - g.y) * (a.x - b.x) - (a.y - b.y) * (f.x - g.x));
                n.x = ((f.x - g.x) * (d.x * a.y - a.x * d.y) - (a.x - d.x) * (g.x * f.y - f.x * g.y)) /
                    ((f.x - g.x) * (a.y - d.y) - (a.x - d.x) * (f.y - g.y));
                n.y = ((f.y - g.y) * (d.y * a.x - a.y * d.x) - (a.y - d.y) * (g.y * f.x - f.y * g.x)) /
                    ((f.y - g.y) * (a.x - d.x) - (a.y - d.y) * (f.x - g.x));
                p.x = ((f.x - c.x) * (d.x * a.y - a.x * d.y) - (a.x - d.x) * (c.x * f.y - f.x * c.y)) /
                    ((f.x - c.x) * (a.y - d.y) - (a.x - d.x) * (f.y - c.y));
                p.y = ((f.y - c.y) * (d.y * a.x - a.y * d.x) - (a.y - d.y) * (c.y * f.x - f.y * c.x)) /
                    ((f.y - c.y) * (a.x - d.x) - (a.y - d.y) * (f.x - c.x));
                importVertexPositions({m, g, h, c, d, p, f, n, a});
            }
            else
            {
                m.x = ((f.x - g.x) * (d.x * a.y - a.x * d.y) - (a.x - d.x) * (g.x * f.y - f.x * g.y)) /
                    ((f.x - g.x) * (a.y - d.y) - (a.x - d.x) * (f.y - g.y));
                m.y = ((f.y - g.y) * (d.y * a.x - a.y * d.x) - (a.y - d.y) * (g.y * f.x - f.y * g.x)) /
                    ((f.y - g.y) * (a.x - d.x) - (a.y - d.y) * (f.x - g.x));
                n.x = ((f.x - g.x) * (d.x * c.y - c.x * d.y) - (c.x - d.x) * (g.x * f.y - f.x * g.y)) /
                    ((f.x - g.x) * (c.y - d.y) - (c.x - d.x) * (f.y - g.y));
                n.y = ((f.y - g.y) * (d.y * c.x - c.y * d.x) - (c.y - d.y) * (g.y * f.x - f.y * g.x)) /
                    ((f.y - g.y) * (c.x - d.x) - (c.y - d.y) * (f.x - g.x));
                p.x = ((h.x - g.x) * (d.x * a.y - a.x * d.y) - (a.x - d.x) * (g.x * h.y - h.x * g.y)) /
                    ((h.x - g.x) * (a.y - d.y) - (a.x - d.x) * (h.y - g.y));
                p.y = ((h.y - g.y) * (d.y * a.x - a.y * d.x) - (a.y - d.y) * (g.y * h.x - h.y * g.x)) /
                    ((h.y - g.y) * (a.x - d.x) - (a.y - d.y) * (h.x - g.x));
                importVertexPositions({b, e, f, n, d, m, g, p, a});
            }
        }
    }
    Polygon::update();
}
void TileShape2::update()
{
}
TileSprite::TileSprite(double lastAngleDeg, double angleDeg, double nextAngleDeg)
{
    m_needToUpdate = true;
    m_twirl = false;
    m_active = false;
    m_trackColor = sf::Color::Black;
    m_trackStyle = AdoCpp::TrackStyle::Standard;
    m_opacity = 100;
    m_shape = TileShape(lastAngleDeg, angleDeg, nextAngleDeg);
    m_angleDeg = angleDeg;
    m_nextAngleDeg = nextAngleDeg;
    m_shape.update();
    m_outline.addVertices(m_shape.exportVertexPositions());
    m_outline.addVertex(m_outline[0].position);
    // m_outline.setClosed(true);
    m_outline.setThickness(0.03f);
    m_outline.update();
    m_twirlShape.setOrigin({0.1f, 0.1f});
    m_twirlShape.setRadius(0.1f);
    m_twirlShape.setOutlineThickness(0.05f);
    m_twirlShape.setFillColor(sf::Color::Transparent);
    m_twirlShape.setOutlineColor(sf::Color::Magenta);
    if (m_nextAngleDeg == 999)
    {
        sf::Vector2f v(1, 0);
        v = v.rotatedBy(sf::degrees(static_cast<float>(m_angleDeg)));
        m_twirlShape.setPosition(v);
    }
    m_speedShape.setOrigin({0.15f, 0.15f});
    m_speedShape.setRadius(0.15f);
    m_speed = 0;
}
void TileSprite::update()
{
    if (!m_needToUpdate)
        return;
    m_needToUpdate = false;
    static sf::Color m_color, m_borderColor;
    switch (m_trackStyle)
    {
    case AdoCpp::TrackStyle::Standard:
        m_color = m_trackColor, m_borderColor = m_color * sf::Color(127, 127, 127, 255);
        break;
    case AdoCpp::TrackStyle::Neon:
        m_color = sf::Color::Black, m_borderColor = m_trackColor;
        break;
    case AdoCpp::TrackStyle::NeonLight:
        m_borderColor = m_trackColor, m_color = m_borderColor * sf::Color(127, 127, 127, 255);
        break;
    case AdoCpp::TrackStyle::Basic:
    case AdoCpp::TrackStyle::Minimal:
    case AdoCpp::TrackStyle::Gems:
        m_color = m_trackColor, m_borderColor = sf::Color::Transparent;
        break;
    }
    m_color.a = static_cast<std::uint8_t>(m_color.a * m_opacity / 100),
    m_borderColor.a = static_cast<std::uint8_t>(m_borderColor.a * m_opacity / 100);
    if (m_active)
        m_color.r = m_color.r / 2, m_color.g = m_color.g / 2 + 255 / 2, m_color.b = m_color.b / 2,
        m_borderColor.r = m_borderColor.r / 2, m_borderColor.g = m_borderColor.g / 2 + 255 / 2,
        m_borderColor.b = m_borderColor.b / 2;
    if (m_shape.getColor() != m_color)
        m_shape.setColor(m_color), m_shape.update();
    if (m_outline.getColor() != m_borderColor)
        m_outline.setColor(m_borderColor), m_outline.update();
    const auto alpha = static_cast<std::uint8_t>(m_opacity / 100 * 255);
    m_twirlShape.setOutlineColor(sf::Color::Magenta * sf::Color(255, 255, 255, alpha));
    if (m_speed)
        m_speedShape.setFillColor((m_speed == 1 ? sf::Color::Red : sf::Color::Blue) * sf::Color(255, 255, 255, alpha));
}
void TileSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_opacity == 0)
        return;
    states.transform *= getTransform();
    states.texture = nullptr;
    target.draw(m_shape, states);
    target.draw(m_outline, states);
    if (m_twirl)
        target.draw(m_twirlShape, states);
    if (m_speed)
        target.draw(m_speedShape, states);
}
// ReSharper disable once CppMemberFunctionMayBeConst
void TileSystem::parse()
{
    double lastAngle, nextAngle;
    m_tileSprites.clear();
    auto& tiles = m_level.tiles;
    const auto& settings = m_level.settings;
    for (size_t i = 0; i < tiles.size(); i++)
    {
        const double angle = tiles[i].angle.deg();

        if (i == tiles.size() - 1)
            nextAngle = angle;
        else
            nextAngle = tiles[i + 1].angle.deg();

        if (i == 0)
            lastAngle = 0;
        else
            lastAngle = tiles[i - 1].angle.deg();

        m_tileSprites.emplace_back(lastAngle, angle, nextAngle);
    }
    double oBpm = settings.bpm, bpm = oBpm;
    for (const auto& tile : tiles)
    {
        for (const auto& event : tile.events)
        {
            if (const auto twirl = std::dynamic_pointer_cast<AdoCpp::Event::GamePlay::Twirl>(event))
            {
                m_tileSprites[twirl->floor].setTwirl(true);
            }
            else if (const auto setSpeed = std::dynamic_pointer_cast<AdoCpp::Event::GamePlay::SetSpeed>(event))
            {
                if (setSpeed->speedType == AdoCpp::Event::GamePlay::SetSpeed::SpeedType::Bpm)
                    bpm = setSpeed->beatsPerMinute;
                else
                    bpm *= setSpeed->bpmMultiplier;
                if (bpm != oBpm)
                    m_tileSprites[setSpeed->floor].setSpeed(bpm > oBpm ? 1 : 2);
                oBpm = bpm;
            }
        }
    }
}
// ReSharper disable once CppMemberFunctionMayBeConst
void TileSystem::update()
{
    auto& tiles = m_level.tiles;
    for (size_t i = 0; i < m_tileSprites.size(); i++)
    {
        // ReSharper disable CppCStyleCast
        auto& sprite = m_tileSprites[i];
        auto& tile = tiles[i];

        sprite.setPosition({(float)tile.pos.c.x, (float)tile.pos.c.y});
        sprite.setActive(m_activeTileIndex ? m_activeTileIndex == i : false);
        // sprite.setTrackColor(sf::Color(tile.color.toInteger()));
        // sprite.setTrackStyle(tile.trackStyle.c);
        sprite.setScale({(float)tile.scale.c.x / 100, (float)tile.scale.c.y / 100});
        sprite.setRotation(sf::degrees((float)tile.rotation.c));
        // sprite.setOpacity((float)tile.opacity);
        sprite.update();
        // ReSharper restore CppCStyleCast
    }
}
void TileSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = nullptr;
    const sf::Vector2f viewCenter(target.getView().getCenter());
    sf::Vector2f viewSize(target.getView().getSize());
    viewSize.x = viewSize.y = (std::max)(viewSize.x, viewSize.y) * 1.5f;
    const sf::FloatRect currentViewRect(viewCenter - viewSize / 2.f, viewSize);
    bool flag = false;
    for (size_t i = m_tileSprites.size() - 1;; i--)
    {
        if (flag)
            break;
        if (i == 0)
            flag = true;
        auto& sprite = m_tileSprites[i];
        const auto& tile = m_level.tiles[i];

        if (currentViewRect.findIntersection(sprite.getGlobalBoundsFaster()) && tile.scale.c.x != 0 &&
            tile.scale.c.y != 0)
        {
            sprite.setTrackColor(sf::Color(tile.color.toInteger()));
            sprite.setTrackStyle(tile.trackStyle.c);
            sprite.setOpacity(static_cast<float>(tile.opacity));
            sprite.update();
            target.draw(m_tileSprites[i]);
        }
    }
    if (m_activeTileIndex && m_tilePlaceMode)
    {
        const auto& selectedTile = m_tileSprites[*m_activeTileIndex];
        const std::map<const char*, float> keyMap = {{"D", 0.f},   {"E", 45.f},  {"W", 90.f},  {"Q", 135.f},
                                                     {"A", 180.f}, {"Z", 225.f}, {"X", 270.f}, {"C", 315.f}};
        const std::map<const char*, float> shiftKeyMap = {{"J", 30.f},  {"Y", 60.f},  {"T", 120.f}, {"H", 150.f},
                                                          {"N", 210.f}, {"V", 240.f}, {"B", 300.f}, {"M", 330.f}};
        const std::map<const char*, float> shiftGraveKeyMap = {{"J", 15.f},  {"Y", 75.f},  {"T", 105.f}, {"H", 165.f},
                                                               {"N", 195.f}, {"V", 255.f}, {"B", 285.f}, {"M", 345.f}};
        const sf::Vector2f tilePos = selectedTile.getPosition();
        for (const auto& [key, value] : (m_tilePlaceMode == 1       ? keyMap
                                             : m_tilePlaceMode == 2 ? shiftKeyMap
                                                                    : shiftGraveKeyMap))
        {
            const sf::Vector2f rectPos = tilePos + sf::Vector2f(1.f, 0.f).rotatedBy(sf::degrees(value));
            sf::Text text{font, key, 128};
            text.setPosition(rectPos);
            text.setOrigin(text.getLocalBounds().getCenter());
            text.setScale(sf::Vector2f(0.002f, -0.002f));
            text.setFillColor(sf::Color::Black);

            sf::RectangleShape rect{{0.5f, 0.5f}};
            rect.setPosition(rectPos - sf::Vector2f(0.25f, 0.25f));
            rect.setFillColor(sf::Color::White);
            target.draw(rect);
            target.draw(text);
        }
    }
}
