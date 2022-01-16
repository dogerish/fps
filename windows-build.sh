#! /bin/bash
# builds a zip of the project for windows
if [[ -z "$1" ]];
then
	"$0" x86_64 && "$0" i686
	exit $?
fi
echo "---- $1"
export CROSS=$1
make || exit $?
rm game-${CROSS}.zip 2> /dev/null
zip dlls-${CROSS}.zip -u game-${CROSS}.exe font.ttf textures/[^gentex]* maps/* -O game-${CROSS}.zip
