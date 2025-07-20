#include <AdoCpp.h>
#include <iostream>
#include <rapidjson/prettywriter.h>

constexpr auto PATH = "F:/Levels/adofaigg2191 [18] The Limit Does Not Exist (By  -K & BamgoeSN)"
                      "_1751702585602/The_limit_does_not_exist.adofai";

void test()
{
    const AdoCpp::Level level{PATH};
    const rapidjson::Document doc = level.intoJson();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer writer(buffer);
    doc.Accept(writer);
    std::cout << buffer.GetString() << std::endl;
}

int main()
{
    // // 1. Import an ADOFAI file to the level.
    // AdoCpp::Level level{PATH};
    // // or "AdoCpp::Level level; level.fromFile(PATH);"
    //
    // // 2. Get some information of the level.
    // std::cout << level.getSettings().artist << " - "
    //           << level.getSettings().song << std::endl;
    //
    // // 3. Parse the level.
    // level.parse();
    //
    // // 4. Get some information of the tiles.
    // for (const auto& tile : level.getTiles())
    //     std::cout << tile.seconds << std::endl;
    //
    // // 5. Update the level.
    // level.update(10); // the 10th second
    //
    // // 6. Get more information of the tiles.
    // for (const auto& tile : level.getTiles())
    // {
    //     // ".o" means "original value".
    //     // Actually, you can get the original value
    //     //     before updating the level.
    //     auto [originalX, originalY] = tile.pos.o;
    //     // ".c" means "current value".
    //     auto [currentX, currentY] = tile.pos.c;
    //     printf("(%.2f, %.2f) (%.2f, %.2f)\n",
    //            originalX, originalY,
    //            currentX,  currentY);
    // }
    //
    // // 7. Modify the level.
    // const auto twirl = new AdoCpp::Event::GamePlay::Twirl();
    // twirl->floor = 1;
    // level.addEvent(twirl, 0);
    // /* events of the 0th tile: {...}         (NOT modified)
    //    events of the 1st tile: {(new)Twirl, ...} (modified)
    //    events of the 2nd tile: {...}         (NOT modified)
    //    ...                                   (NOT modified)
    //  */
    //
    // // After modifying, the level is not parsed.
    // assert(level.isParsed() == false);
    //
    // // 8. Export the level as JSON (needn't parse).
    // rapidjson::Document doc = level.intoJson();
    // /* ... */

    test();

    return 0;
}
