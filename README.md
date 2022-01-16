# fps - First Person Raycaster
Raycaster made by me, algorithms inspired by online sources.

* If you have ideas, you can post them on [this Google Doc](https://docs.google.com/document/d/1FT-uGfJ4u9irJ9EBzfeAzoy5IvE1mD8UnRtnMjZ8P5U/edit?usp=sharing). Please follow the guidelines posted there.
* Uses C++ and SDL 2
* Support for Windows: extract `game-i686.zip` (32-bit) or `game-x86_64.zip` (64-bit) and run the executable. If you have problems, create an issue.
* As of 2022-01-14, this works on Linux. If you are using g++ < 9, uncomment the `-lstdc++fs` so that it has the standard file system library.
* `make` to compile. Comment the `--debug` in like the third line of the makefile and then `make redo` to get a non-debug binary, which might be a bit faster.
	* Cross compiling may or may not work for you. It works for me on a Mac with mingw.
	* Set the environment variable CROSS to either i686 (32-bit) or x86\_64 (64-bit) to cross compile.
	* The windows-build shell script builds the zips automatically when run
* Textures that aren't lame are from Cube 2: Sauerbraten. Check out the `locations.txt` file there.

## Blog I Guess
1. [![video](thumbnails/2021-12-21%20at%2011.29.36.png)](https://streamable.com/1bni67)<br/>
   2021-12-21 at 11.29.36: Single ray is stopping at the right time.

2. [![video](thumbnails/2021-12-21%20at%2013.36.45.png)](https://streamable.com/8rzq6w)<br/>
   2021-12-21 at 13.36.45: Multiple rays cast at once in a 2D environment.

3. [![video](thumbnails/2021-12-21%20at%2015.13.59.png)](https://streamable.com/zj4k5w)<br/>
   2021-12-21 at 15.13.59: Mapping to columns to simulate 3D is working, but the fish eye fix is not

4. [![video](thumbnails/2021-12-23%20at%2009.28.56.png)](https://streamable.com/jn3lz4)<br/>
   2021-12-23 at 09.28.56: Fisheye is fixed but now there's a problem with slight angles going through walls.

5. [![video](thumbnails/wall%20glitch%20fixed.png)](https://streamable.com/n2d3hd)<br/>
   2021-12-23 at 11.55.53: Wall glitch fixed.

6. [![video](thumbnails/face%20shading.png)](https://streamable.com/l4rvg9)<br/>
   2021-12-23 at 17.08.09: Face shading added

7. [![video](thumbnails/2021-12-24%20at%2011.03.21.png)](https://streamable.com/sly426)<br/>
   2021-12-24 at 11.03.21: Movement and turning is improved, and face shading supports more than 2 sides.

8. [![video](thumbnails/tripping%20on%20acid.png)](https://streamable.com/hoj2i8)<br/>
   2021-12-24 at 12.20.30: Tried adding a sky and ground

9. [![video](thumbnails/collisions%20added.png)](https://streamable.com/gi91iv)<br/>
   2021-12-24 at 13.39.04: Added collisions

10. [![video](thumbnails/single%20color%20textures.png)](https://streamable.com/tvulao)<br/>
   2021-12-24 at 17.02.29: Added single color textures<br/>
   ![failed textures](thumbnails/badtextures.png)

11. [![video](thumbnails/2021-12-24%20at%2017.28.25.png)](https://youtu.be/v8TDuYdmmj4)<br/>
   2021-12-24 at 17.28.25: Textures are working<br/>
   ![textures with fog](thumbnails/fogtextures.png)

12. [![video](thumbnails/2021-12-26%20at%2016.22.01.png)](https://streamable.com/r5fnpo)<br/>
   2021-12-26 at 16.22.01: Added floor and ceiling, but now it's very slow.

13. [![video](thumbnails/2021-12-26%20at%2019.18.28.png)](https://streamable.com/uyyh0k)<br/>
   2021-12-26 at 19.18.28: Optimized the floors and ceiling

14. [![video](thumbnails/2021-12-28%20at%2010.39.13.png)](https://streamable.com/j6oz31)<br/>
   2021-12-28 at 10.39.13: Added display of heading and fps

15. [![video](thumbnails/2021-12-28%20at%2012.50.11.png)](https://streamable.com/fznbon)<br/>
   2021-12-28 at 12.50.11: Refactored to use surfaces (CPU-based rendering). Much more efficient for this ray caster.

16. [![video](thumbnails/2021-12-28%20at%2014.41.30.png)](https://streamable.com/1js8uo)<br/>
   2021-12-28 at 14.41.30: Sort of fixed the bug with close walls being glitched

17. [![video](thumbnails/2021-12-29%20at%2013.05.33.png)](https://streamable.com/otvhna)<br/>
   2021-12-29 at 13.05.33: Added texture editing

18. [![video](thumbnails/toaster.png)](https://streamable.com/ffg9u2)<br/>
   2021-12-31 at 14.38.47: Game running on Windows! *Video by J-Toastie*

19. [![video](thumbnails/2022-01-03%20at%2016.42.25.png)](https://streamable.com/55nhaf)<br/>
   2022-01-03 at 16.42.25: Added some GUI stuff and input boxes

20. [![video](thumbnails/2022-01-05%20at%2021.20.41.png)](https://streamable.com/epsh6q)<br/>
   2022-01-05 at 21.20.41: Fleshed out GUI system

21. [![video](thumbnails/2022-01-16%20at%2012.38.08.png)](https://streamable.com/x6a758)<br/>
   2022-01-16 at 12.38.08: Map listing, wall editor, new map button

