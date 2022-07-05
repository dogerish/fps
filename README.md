# fps - First Person Raycaster
Raycaster made by me, algorithms inspired by online sources.

Builds can be found in the releases/ folder. Download the zip, extract it, and 
run the included batch file.

* If you have ideas, you can post them on [this Google 
  Doc](https://docs.google.com/document/d/1FT-uGfJ4u9irJ9EBzfeAzoy5IvE1mD8UnRtnMjZ8P5U/edit?usp=sharing). 
  Please follow the guidelines posted there.
* Uses C++ and SDL 2
* As of 2022-01-14, this works on Linux. If you are using g++ < 9, uncomment 
  the `-lstdc++fs` so that it has the standard file system library.
* `make` to compile. Comment the `--debug` in like the third line of the 
  makefile and then `make deepclean all` to get a non-debug binary, which might 
  be a bit faster.
	* Cross compiling may or may not work for you. It works for me on a Mac 
	  with mingw.
	* Set the environment variable CROSS to either i686 (32-bit) or x86\_64 
	  (64-bit) to cross compile - `make CROSS=i686`
	* The `release` target for make uses the `make-release` script to craft the 
	  release zip. Files are included along with resources and the executable 
	  from the release-includes directory.
* Textures that aren't lame are from Cube 2: Sauerbraten. Check out the 
  `locations.txt` file there.

## Blog I Guess
Blog I Guess has been moved to GitHub Pages! It's so much better, go check it 
out at <https://dogerish.github.io/fps/>!
