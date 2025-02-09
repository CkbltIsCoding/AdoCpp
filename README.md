# AdoCpp & AdoCppGame

## AdoCpp Introduction

AdoCpp is a C++ library for parsing .adofai files.
It can calculate the beat of each tile, update the level by time and so on.

## AdoCpp Usage

The code below shows us how to get the beat of each tile and print it.

```cpp
#include <iostream>
#include <AdoCpp>
int main()
{
    AdoCpp::Level level("./Multi_arm/main.adofai");
    level.parse();
    for (auto& tile : level.tiles)
    {
        std::cout << tile.beat << std::endl;
    }
    return 0;
}
```

## AdoCppGame Introduction

AdoCppGame is a game made in C++.
Like ADOFAI, AdoCppGame can load .adofai files and play it.
