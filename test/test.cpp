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

int main()
{
    using namespace std;

    AdoCpp::Level level{path};
    level.parse();

    cout << level.getSettings().bpm << endl;

    return 0;
}
