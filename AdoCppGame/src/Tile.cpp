#include "Tile.h"
#include <map>

static constexpr float PI = 3.14159265358979323846, outline = 0.02f;

static bool pointIsInsideTriangle(const std::vector<sf::Vector2f>& points, const sf::Vector2f point)
{
    //////////////////////////////////////////////////////////////////////////////
    //
    // Selba Ward (https://github.com/Hapaxia/SelbaWard)
    // --
    //
    // Polygon
    //
    // Copyright(c) 2022-2025 M.J.Silk
    //
    // This software is provided 'as-is', without any express or implied
    // warranty. In no event will the authors be held liable for any damages
    // arising from the use of this software.
    //
    // Permission is granted to anyone to use this software for any purpose,
    // including commercial applications, and to alter it and redistribute it
    // freely, subject to the following restrictions :
    //
    // 1. The origin of this software must not be misrepresented; you must not
    // claim that you wrote the original software.If you use this software
    // in a product, an acknowledgment in the product documentation would be
    // appreciated but is not required.
    //
    // 2. Altered source versions must be plainly marked as such, and must not be
    // misrepresented as being the original software.
    //
    // 3. This notice may not be removed or altered from any source distribution.
    //
    // M.J.Silk
    // MJSilk2@gmail.com
    //
    //////////////////////////////////////////////////////////////////////////////
    const long double point1X{(points[0].x)};
    const long double point1Y{(points[0].y)};
    const long double point2X{(points[1].x)};
    const long double point2Y{(points[1].y)};
    const long double point3X{(points[2].x)};
    const long double point3Y{(points[2].y)};
    const long double pointX{(point.x)};
    const long double pointY{(point.y)};

    const long double denominatorMultiplier{
        1.l / ((point2Y - point3Y) * (point1X - point3X) + (point3X - point2X) * (point1Y - point3Y))};
    const long double a{((point2Y - point3Y) * (pointX - point3X) + (point3X - point2X) * (pointY - point3Y)) *
                        denominatorMultiplier};
    const long double b{((point3Y - point1Y) * (pointX - point3X) + (point1X - point3X) * (pointY - point3Y)) *
                        denominatorMultiplier};
    const long double c{1.l - a - b};
    return a >= 0.l && a <= 1.l && b >= 0.l && b <= 1.l && c >= 0.l && c <= 1.l;
}

// Thanks for StArray's code
static void createCircle(sf::Vector3f center, const float r, bool isBorder, std::vector<sf::Vector3f>& vertices,
                         std::vector<size_t>& m_triangles, std::vector<bool>& borders, int resolution)
{
    if (resolution <= 0)
    {
        resolution = 32; // Default value if not provided
    }

    const int centerIndex = vertices.size();
    vertices.push_back(sf::Vector3f(center));
    borders.push_back(isBorder);

    for (int i = 0; i < resolution; i++)
    {
        const float angle = 2.f * PI * i / resolution;
        sf::Vector3f vertex = sf::Vector3f(cos(angle) * r, sin(angle) * r, 0) + center;
        vertices.push_back(vertex);
        borders.push_back(isBorder);
    }

    for (int i = 1; i < resolution; i++)
    {
        m_triangles.push_back(centerIndex);
        m_triangles.push_back(centerIndex + i);
        m_triangles.push_back(centerIndex + i + 1);
    }

    // Closing the circle by connecting the last vertex to the first
    m_triangles.push_back(centerIndex);
    m_triangles.push_back(centerIndex + resolution);
    m_triangles.push_back(centerIndex + 1);
}

// Thanks for StArray's code
static void createTileMesh(float width, float length, sf::Angle startAngle, sf::Angle endAngle,
                           sf::VertexArray& m_vertices, std::vector<size_t>& m_triangles, std::vector<bool>& m_borders)
{
    startAngle = startAngle.wrapUnsigned(), endAngle = endAngle.wrapUnsigned();
    std::vector<sf::Vector3f> vertices;
    std::vector<bool> borders;
    m_triangles.clear();

    // region basic process
    const float m11 = cos(startAngle.asRadians()), m12 = sin(startAngle.asRadians()), m21 = cos(endAngle.asRadians()),
                m22 = sin(endAngle.asRadians());
    float a[2]{};

    if ((startAngle - endAngle).wrapUnsigned() >= (endAngle - startAngle).wrapUnsigned())
    {
        a[0] = startAngle.wrapUnsigned().asRadians();
        a[1] = a[0] + (endAngle - startAngle).wrapUnsigned().asRadians();
    }
    else
    {
        a[0] = endAngle.wrapUnsigned().asRadians();
        a[1] = a[0] + (startAngle - endAngle).wrapUnsigned().asRadians();
    }
    float angle = a[1] - a[0], mid = a[0] + angle / 2.f;
    // endregion
    if (angle < 2.0943952f && angle > 0)
    {
        // region angle < 2.0943952
        float x;
        if (angle < 0.08726646f)
        {
            x = 1.f;
        }
        else if (angle < 0.5235988f)
        {
            x = std::lerp(1.f, 0.83f, pow((angle - 0.08726646f) / 0.43633235f, 0.5f));
        }
        else if (angle < 0.7853982f)
        {
            x = std::lerp(0.83f, 0.77f, pow((angle - 0.5235988f) / 0.2617994f, 1.f));
        }
        else if (angle < 1.5707964f)
        {
            x = std::lerp(0.77f, 0.15f, pow((angle - 0.7853982f) / 0.7853982f, 0.7f));
        }
        else
        {
            x = std::lerp(0.15f, 0.f, pow((angle - 1.5707964f) / 0.5235988f, 0.5f));
        }
        float distance;
        float radius;
        if (x == 1.f)
        {
            distance = 0.f;
            radius = width;
        }
        else
        {
            radius = std::lerp(0.f, width, x);
            distance = (width - radius) / sin(angle / 2.f);
        }
        float circlex = -distance * cos(mid);
        float circley = -distance * sin(mid);
        width += outline;
        length += outline;
        radius += outline;
        createCircle(sf::Vector3f(circlex, circley, 0), radius, true, vertices, m_triangles, borders, 0);
        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(-radius * sin(a[1]) + circlex, radius * cos(a[1]) + circley, 0));
            vertices.push_back(sf::Vector3f(circlex, circley, 0));
            vertices.push_back(sf::Vector3f(radius * sin(a[0]) + circlex, -radius * cos(a[0]) + circley, 0));
            vertices.push_back(sf::Vector3f(width * sin(a[0]), -width * cos(a[0]), 0));
            vertices.push_back(sf::Vector3f());
            vertices.push_back(sf::Vector3f(-width * sin(a[1]), width * cos(a[1]), 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count + 4);
            for (int i = 0; i < 6; i++)
                borders.push_back(true);
        }
        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(length * m11 + width * m12, length * m12 - width * m11, 0));
            vertices.push_back(sf::Vector3f(length * m11 - width * m12, length * m12 + width * m11, 0));
            vertices.push_back(sf::Vector3f(-width * m12, width * m11, 0));
            vertices.push_back(sf::Vector3f(width * m12, -width * m11, 0));

            vertices.push_back(sf::Vector3f(length * m21 + width * m22, length * m22 - width * m21, 0));
            vertices.push_back(sf::Vector3f(length * m21 - width * m22, length * m22 + width * m21, 0));
            vertices.push_back(sf::Vector3f(-width * m22, width * m21, 0));
            vertices.push_back(sf::Vector3f(width * m22, -width * m21, 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 7);
            m_triangles.push_back(count + 4);
            for (int i = 0; i < 8; i++)
                borders.push_back(true);
        }
        // endregion
        // region outline
        width -= outline * 2.f;
        length -= outline * 2.f;
        radius -= outline * 2.f;
        if (radius < 0)
        {
            radius = 0;
            circlex = -width / sin(angle / 2.f) * cos(mid);
            circley = -width / sin(angle / 2.f) * sin(mid);
        }
        createCircle(sf::Vector3f(circlex, circley, 0), radius, false, vertices, m_triangles, borders, 0);
        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(-radius * sin(a[1]) + circlex, radius * cos(a[1]) + circley, 0));
            vertices.push_back(sf::Vector3f(circlex, circley, 0));
            vertices.push_back(sf::Vector3f(radius * sin(a[0]) + circlex, -radius * cos(a[0]) + circley, 0));
            vertices.push_back(sf::Vector3f(width * sin(a[0]), -width * cos(a[0]), 0));
            vertices.push_back(sf::Vector3f());
            vertices.push_back(sf::Vector3f(-width * sin(a[1]), width * cos(a[1]), 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count + 4);
            for (int i = 0; i < 6; i++)
                borders.push_back(false);
        }
        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(length * m11 + width * m12, length * m12 - width * m11, 0));
            vertices.push_back(sf::Vector3f(length * m11 - width * m12, length * m12 + width * m11, 0));
            vertices.push_back(sf::Vector3f(-width * m12, width * m11, 0));
            vertices.push_back(sf::Vector3f(width * m12, -width * m11, 0));

            vertices.push_back(sf::Vector3f(length * m21 + width * m22, length * m22 - width * m21, 0));
            vertices.push_back(sf::Vector3f(length * m21 - width * m22, length * m22 + width * m21, 0));
            vertices.push_back(sf::Vector3f(-width * m22, width * m21, 0));
            vertices.push_back(sf::Vector3f(width * m22, -width * m21, 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 7);
            m_triangles.push_back(count + 4);
            for (int i = 0; i < 8; i++)
                borders.push_back(false);
        }
        // endregion
    }
    else if (angle > 0)
    {
        // region normal case
        width += outline;
        length += outline;

        float circlex = -width / sin(angle / 2.f) * cos(mid);
        float circley = -width / sin(angle / 2.f) * sin(mid);

        {
            const size_t count = 0;
            vertices.push_back(sf::Vector3f(circlex, circley, 0));
            vertices.push_back(sf::Vector3f(width * sin(a[0]), -width * cos(a[0]), 0));
            vertices.push_back(sf::Vector3f());
            vertices.push_back(sf::Vector3f(-width * sin(a[1]), width * cos(a[1]), 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            for (int i = 0; i < 4; i++)
                borders.push_back(true);
        }
        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(length * m11 + width * m12, length * m12 - width * m11, 0));
            vertices.push_back(sf::Vector3f(length * m11 - width * m12, length * m12 + width * m11, 0));
            vertices.push_back(sf::Vector3f(-width * m12, width * m11, 0));
            vertices.push_back(sf::Vector3f(width * m12, -width * m11, 0));

            vertices.push_back(sf::Vector3f(length * m21 + width * m22, length * m22 - width * m21, 0));
            vertices.push_back(sf::Vector3f(length * m21 - width * m22, length * m22 + width * m21, 0));
            vertices.push_back(sf::Vector3f(-width * m22, width * m21, 0));
            vertices.push_back(sf::Vector3f(width * m22, -width * m21, 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 7);
            m_triangles.push_back(count + 4);
            for (int i = 0; i < 8; i++)
                borders.push_back(true);
        }
        // endregion
        // region fill
        width -= outline * 2.f;
        length -= outline * 2.f;

        circlex = -width / sin(angle / 2.f) * cos(mid);
        circley = -width / sin(angle / 2.f) * sin(mid);

        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(circlex, circley, 0));
            vertices.push_back(sf::Vector3f(width * sin(a[0]), -width * cos(a[0]), 0));
            vertices.push_back(sf::Vector3f());
            vertices.push_back(sf::Vector3f(-width * sin(a[1]), width * cos(a[1]), 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            for (int i = 0; i < 4; i++)
                borders.push_back(false);
        }
        {
            const size_t count = vertices.size();
            vertices.push_back(sf::Vector3f(length * m11 + width * m12, length * m12 - width * m11, 0));
            vertices.push_back(sf::Vector3f(length * m11 - width * m12, length * m12 + width * m11, 0));
            vertices.push_back(sf::Vector3f(-width * m12, width * m11, 0));
            vertices.push_back(sf::Vector3f(width * m12, -width * m11, 0));

            vertices.push_back(sf::Vector3f(length * m21 + width * m22, length * m22 - width * m21, 0));
            vertices.push_back(sf::Vector3f(length * m21 - width * m22, length * m22 + width * m21, 0));
            vertices.push_back(sf::Vector3f(-width * m22, width * m21, 0));
            vertices.push_back(sf::Vector3f(width * m22, -width * m21, 0));
            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            m_triangles.push_back(count + 4);
            m_triangles.push_back(count + 5);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 6);
            m_triangles.push_back(count + 7);
            m_triangles.push_back(count + 4);
            for (int i = 0; i < 8; i++)
                borders.push_back(false);
        }
        // endregion
    }
    else
    {
        // region outline (if included angle == 180, draw a semicircle)
        length = width;
        width += outline;
        length += outline;

        const sf::Vector3f midpoint{-m11 * 0.04f, -m12 * 0.04f, 0};
        createCircle(midpoint, width, true, vertices, m_triangles, borders, 0);

        {
            const size_t count = vertices.size();
            vertices.push_back(midpoint + sf::Vector3f(length * m11 + width * m12, length * m12 - width * m11, 0));
            vertices.push_back(midpoint + sf::Vector3f(length * m11 - width * m12, length * m12 + width * m11, 0));
            vertices.push_back(midpoint + sf::Vector3f(-width * m12, width * m11, 0));
            vertices.push_back(midpoint + sf::Vector3f(width * m12, -width * m11, 0));

            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            for (int i = 0; i < 4; i++)
                borders.push_back(true);
        }
        // endregion
        // region fill
        width -= outline * 2.f;
        length -= outline * 2.f;
        createCircle(midpoint, width, false, vertices, m_triangles, borders, 0);
        {
            const size_t count = vertices.size();
            vertices.push_back(midpoint + (sf::Vector3f(length * m11 + width * m12, length * m12 - width * m11, 0)));
            vertices.push_back(midpoint + (sf::Vector3f(length * m11 - width * m12, length * m12 + width * m11, 0)));
            vertices.push_back(midpoint + (sf::Vector3f(-width * m12, width * m11, 0)));
            vertices.push_back(midpoint + (sf::Vector3f(width * m12, -width * m11, 0)));

            m_triangles.push_back(count);
            m_triangles.push_back(count + 1);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 2);
            m_triangles.push_back(count + 3);
            m_triangles.push_back(count);
            for (int i = 0; i < 4; i++)
                borders.push_back(false);
        }
        // endregion
    }
    m_vertices.clear();
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_borders.clear();
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        const size_t idx = m_triangles[i];
        m_vertices.append(sf::Vertex(sf::Vector2f(vertices[idx].x, vertices[idx].y)));
        m_borders.push_back(borders[idx]);
    }
}
// Thanks for StArray's code
static void createMidSpinMesh(float width, sf::Angle a1, sf::VertexArray& m_vertices, std::vector<size_t>& m_triangles,
                              std::vector<bool>& m_borders)
{
    a1 = a1.wrapUnsigned();
    float length = width;
    const float m1 = cos(a1.asRadians()), m2 = sin(a1.asRadians());

    // region outline
    std::vector<sf::Vector3f> vertices;
    std::vector<bool> borders;
    m_triangles.clear();
    const sf::Vector3f midpoint{-m1 * 0.04f, -m2 * 0.04f, 0};
    width += outline;
    length += outline;
    {
        constexpr size_t count = 0;
        vertices.push_back(midpoint + sf::Vector3f(length * m1 + width * m2, length * m2 - width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(length * m1 - width * m2, length * m2 + width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(-width * m2, width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(width * m2, -width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(-width * m1, -width * m2, 0));
        vertices.push_back(midpoint + sf::Vector3f(width * m2, -width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(-width * m2, width * m1, 0));
        m_triangles.push_back(count);
        m_triangles.push_back(count + 1);
        m_triangles.push_back(count + 2);
        m_triangles.push_back(count + 2);
        m_triangles.push_back(count + 3);
        m_triangles.push_back(count);
        m_triangles.push_back(count + 4);
        m_triangles.push_back(count + 5);
        m_triangles.push_back(count + 6);
        for (int i = 0; i < 7; i++)
            borders.push_back(true);
    }
    // endregion
    // region fill
    width -= outline * 2;
    length -= outline * 2;
    {
        const size_t count = vertices.size();
        vertices.push_back(midpoint + sf::Vector3f(length * m1 + width * m2, length * m2 - width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(length * m1 - width * m2, length * m2 + width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(-width * m2, width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(width * m2, -width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(-width * m1, -width * m2, 0));
        vertices.push_back(midpoint + sf::Vector3f(width * m2, -width * m1, 0));
        vertices.push_back(midpoint + sf::Vector3f(-width * m2, width * m1, 0));
        m_triangles.push_back(count);
        m_triangles.push_back(count + 1);
        m_triangles.push_back(count + 2);
        m_triangles.push_back(count + 2);
        m_triangles.push_back(count + 3);
        m_triangles.push_back(count);
        m_triangles.push_back(count + 4);
        m_triangles.push_back(count + 5);
        m_triangles.push_back(count + 6);
        for (int i = 0; i < 7; i++)
            borders.push_back(false);
    }
    // endregion

    m_vertices.clear();
    m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_borders.clear();
    for (size_t i = 0; i < m_triangles.size(); i++)
    {
        const size_t idx = m_triangles[i];
        m_vertices.append(sf::Vertex(sf::Vector2f(vertices[idx].x, vertices[idx].y)));
        m_borders.push_back(borders[idx]);
    }
}

TileShape::TileShape(const double l_lastAngle, const double l_angle, const double l_nextAngle)
{
    m_lastAngle = l_lastAngle, m_angle = l_angle, m_nextAngle = l_nextAngle;
    m_interpolationLevel = 16;
}
void TileShape::update()
{
    std::vector<size_t> m_triangles{};
    if (m_nextAngle == 999)
        createMidSpinMesh(0.275f, sf::degrees(m_angle + 180), m_vertices, m_triangles, m_borders);
    else
    {
        const sf::Angle startAngle = sf::degrees(m_angle == 999 ? m_lastAngle : m_angle + 180).wrapUnsigned(),
                        endAngle = sf::degrees(m_nextAngle).wrapUnsigned();
        createTileMesh(0.275f, 0.5f, startAngle, endAngle, m_vertices, m_triangles, m_borders);
    }
    m_bounds = m_vertices.getBounds();
}
sf::FloatRect TileShape::getLocalBounds() const { return m_bounds; }
sf::FloatRect TileShape::getGlobalBounds() const { return getTransform().transformRect(getLocalBounds()); }
bool TileShape::isPointInside(const sf::Vector2f point) const
{
    for (size_t i = 0; i < m_vertices.getVertexCount(); i += 3)
    {
        if (pointIsInsideTriangle({m_vertices[i].position, m_vertices[i + 1].position, m_vertices[i + 2].position},
                                  point))
            return true;
    }
    return false;
}
void TileShape::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = nullptr;
    target.draw(m_vertices, states);
}
sf::Color TileShape::getFillColor() const { return m_fillColor; }
void TileShape::setFillColor(const sf::Color color)
{
    m_fillColor = color;
    for (size_t i = 0; i < m_borders.size(); i++)
        if (!m_borders[i])
            m_vertices[i].color = m_fillColor;
}
sf::Color TileShape::getOutlineColor() const { return m_outlineColor; }
void TileShape::setOutlineColor(const sf::Color color)
{
    m_outlineColor = color;
    for (size_t i = 0; i < m_borders.size(); i++)
        if (m_borders[i])
            m_vertices[i].color = m_outlineColor;
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
    {
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
    }
    {
        m_speedShape.setOrigin({0.15f, 0.15f});
        m_speedShape.setRadius(0.15f);
        m_speed = 0;
    }
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
    // if (m_shape.getFillColor() != m_color)
    m_shape.setFillColor(m_color);
    // if (m_shape.getOutlineColor() != m_borderColor)
    m_shape.setOutlineColor(m_borderColor);
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
    // target.draw(m_outline, states);
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
    const auto& tiles = m_level.tiles;
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
        const auto& tile = tiles[i];

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
