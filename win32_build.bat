cl main.cpp /I packages\SDL2\include^
 -link -opt:ref packages/SDL2/lib/x64/SDL2main.lib packages/SDL2/lib/x64/SDL2.lib^
 /out:win64_build\raycasting_engine.exe