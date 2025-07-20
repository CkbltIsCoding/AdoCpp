#include <AdoCpp.h>
#include <chrono>
#include <iostream>
#include <thread>

constexpr auto path = "F:/Levels/adofaigg2191 [18] The Limit Does Not Exist (By  -K & BamgoeSN)"
                      "_1751702585602/The_limit_does_not_exist.adofai";

std::filesystem::path getPath()
{
    // std::string str;
    // std::cout << "Please enter the path of the file: ";
    // std::getline(std::cin, str);
    // return str;
    return path;
}

int main()
{
    const std::filesystem::path path = getPath();
    AdoCpp::Level level{path};
    level.parse();
    const auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 1; i < level.getTiles().size(); i++)
    {
        auto duration = std::chrono::milliseconds(static_cast<long long>(level.getTiles()[i].seconds * 1000));
        std::this_thread::sleep_until(start + duration);
        printf("floor: %llu sec: %.3f\n", i, level.getTiles()[i].seconds);
    }
    return 0;
}
