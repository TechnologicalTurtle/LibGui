# LibGui

>[!CAUTION]
>This project was made to learn OpenGL, Git, CMake, some advanced C++ and making bigger codebases, so it's not intended for real usage.

LibGui is multiplatform GUI C++ library with text rendering/editing, image loading/rendering/editing and much more.

**Here is [documentetion](https://github.com/TechnologicalTurtle/LibGui/wiki).**

## Credits
This library was made by me, [TechnologicalTurtle](https://github.com/TechnologicalTurtle), however I used some cool libraries not made by me:
- [glfw](glfw.org)
- [glad](https://github.com/Dav1dde/glad), which I know isn't technically library but loader for [OpenGL](https://www.opengl.org), but hey, it still should be credited.
- [freetype](https://freetype.org)
- [stb_image and stb_image_write](https://github.com/nothings/stb)
- And speciall thanks to [learnopengl.com](https://learnopengl.com) for amazing tutorials on OpenGL.
## About
I created this project mainly to learn wotŕking with tools like git or cmake, also to try writing some 'good' code. After few months of on and off work, I finished it, but I realized that it's maybe too big to ask random redditors.  
Also excuse my english, as I'm still in middle school. (almost high-school)
### My goals with this project
- Write _cool_ GUI OpenGL library.
- Write few test apps with it.
- Ask people from reddit, where should I improve.
- [Ideally not use AI at all.](#usage-of-ai)
### Usage of AI
How I mentioned, I didn't want to use AI at all, because I don't want to became dependent on it (and also when I used AI in older projects, I always had to say "Look what I created! Ehh yeah I used AI, but not that much!"). So did I succeed? Kinda, in this code isn't anything from me written by AI, but I had to use it like four times, when after hours of googling, I couldn't find answer, here are two instances I remember:
- [this one's stupid, I know] I was coding too long and my brain got fried, then OpenGL suddenly didn't want to render anything in one specific instance. Yes, it was that all points were same, so the triangle was infinitely small.
- Here I think, it was the right thing, as the bug occured only in C++20 and higher, and I couln't find anything about it.
  I think it was something like ambiguous operator== order problem.
