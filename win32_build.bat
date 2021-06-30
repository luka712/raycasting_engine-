cl^
 main.cpp graphics.cpp map.cpp player.cpp ray.cpp wall.cpp upng.cpp textures.cpp^
 /I packages\SDL2\include^
 -link -opt:ref packages/SDL2/lib/x64/SDL2main.lib packages/SDL2/lib/x64/SDL2.lib^
 /out:win64_build\raycasting_engine.exe