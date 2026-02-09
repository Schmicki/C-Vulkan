# C-Vulkan

⚠️ Status: Archived ⚠️ This project was created for educational purposes and is no longer maintained.

A minimalist C99 vulkan renderer.

## Screenshot

![](resources/Screenshot-01.png)

## Features

- dynamic containers ([containers.h](./src/containers.h), [containers.c](./src/containers.c))
- linear algebra ([math.h](./src/math.h), [math.c](./src/math.c))
- fullscreen window using win32 ([window.h](./src/window.h), [window.c](./src/window.c))
- basic vulkan rendering ([rendering.h](./src/rendering.h), [rendering.c](./src/rendering.c))

## How to build

### Dependencies

- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

### Windows

While in project root:

1. run `premake-visual-studio.bat` in terminal
2. open `CVulkan.sln` in Visual Studio
3. select config and platform
4. build & run

## License

[MIT License](./LICENSE)
