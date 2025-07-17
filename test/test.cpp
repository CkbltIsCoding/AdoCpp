#include <AdoCpp.h>
#include <iostream>
#include <rapidjson/prettywriter.h>

constexpr auto path = "F:/Levels/adofaigg2191 [18] The Limit Does Not Exist (By  -K & BamgoeSN)"
                      "_1751702585602/The_limit_does_not_exist.adofai";

void test()
{
    const AdoCpp::Level level{path};
    const rapidjson::Document doc = level.intoJson();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    doc.Accept(writer);
    std::cout << buffer.GetString() << std::endl;
}

#include <iostream>
#include <AdoCpp.h>

int main()
{
    AdoCpp::Level level{path};

    // 2. Get some information of the level.
    std::cout << level.getSettings().artist << " - "
              << level.getSettings().song << std::endl;

    // 3. Parse the level.
    level.parse();

    // 4. Get some information of the tiles.
    for (const auto& tile : level.getTiles())
        std::cout << tile.seconds << std::endl;

    // 5. Update the level.
    level.update(10); // the 10th second

    // 6. Get more information of the tiles.
    for (const auto& tile : level.getTiles())
    {
        // ".o" means "original value".
        // Actually, you can get the original value
        //     before updating the level.
        auto [originalX, originalY] = tile.pos.o;
        // ".c" means "current value".
        auto [currentX, currentY] = tile.pos.c;
        printf("(%.2f, %.2f) (%.2f, %.2f)\n", originalX, originalY, currentX, currentY);
    }

    // 7. Export the level as JSON.
    rapidjson::Document doc = level.intoJson();
    /* ... */

    return 0;
}
