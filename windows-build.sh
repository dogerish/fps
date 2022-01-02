#! /bin/bash
# builds a zip of the project for windows
make CROSS=WIN32
rm game-win32.zip 2> /dev/null
zip dlls.zip -u game.exe Sans.ttf textures/*.bmp maps/* -O game-win32.zip