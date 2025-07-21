# AdoCpp

## Introduction

AdoCpp is an ADOFAI level parser and generator for C++.

AdoCpp uses following software as its dependencies:

- CMake as a general build tool
- Rapidjson to parse JSON files
- (optional) Doxygen to build documentation

## Usage at a glance

```c++
#include <iostream>
#include <AdoCpp.h>

int main()
{
    // 1. Import an ADOFAI file to the level.
    constexpr const char* PATH = "...";
    AdoCpp::Level level{PATH};
    // or "AdoCpp::Level level; level.fromFile(PATH);"
    
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
        printf("(%.2f, %.2f) (%.2f, %.2f)\n",
               originalX, originalY,
               currentX,  currentY);
    }
    
    // 7. Modify the level.
    const auto twirl = new AdoCpp::Event::GamePlay::Twirl();
    twirl->floor = 1;
    level.addEvent(twirl, 0);
    /* events of the 0th tile: {...}         (NOT modified)
       events of the 1st tile: {(new)Twirl, ...} (modified)
       events of the 2nd tile: {...}         (NOT modified)
       ...                                   (NOT modified)
     */

    // After modifying, the level is not parsed.
    assert(level.isParsed() == false);

    // 8. Export the level as JSON (needn't parse).
    rapidjson::Document doc = level.intoJson();
    /* ... */
    
    return 0;
}
```

Notice that AdoCpp is still under development
and it is a little buggy.

---

# AdoCppGame

## Introduction

AdoCppGame is an ADOFAI level editor (cannot edit though) and player.

AdoCppGame uses following software and libraries as its dependencies:

- CMake as a general build tool
- AdoCpp to parse ADOFAI files
- SFML as a game engine
- Selbaward to draw polygons with SFML
- ImGui as a GUI library
- ImGui-SFML to use ImGui with SFML
- ImGuiFileDialog to select files
- ImPlot to show audio oscillogram
- Exprtk as a mathematical expression toolkit library

Notice that AdoCppGame is still under development
and it is a little buggy.
