#include <AdoCpp.h>
#include <iostream>
#include <rapidjson/prettywriter.h>

int main()
{
    const AdoCpp::Level level{"F:/Levels/adofaigg2191 [18] The Limit Does Not Exist (By  -K & BamgoeSN)"
                      "_1751702585602/The_limit_does_not_exist.adofai"};
    const rapidjson::Document doc = level.intoJson();
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter writer(buffer);
    doc.Accept(writer);
    std::cout << buffer.GetString() << std::endl;

    return 0;
}
