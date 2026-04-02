# Tether
Tether is a app creation/GUI library created in C++. 

# How to contribute

*Naming conventions*

* **Class/Enums: CamelCase**
* **Variable: lowerCamelCase**
* **Function: CamelCase**
* **Enum value: SNAKE_CASE**

# CMake options
* TETHER_BUILD_TESTS : Enable if the tests should be built
* TETHER_BUILD_AS_SHARED : Enabled by default. Disable if tether should be built as a
	static library.

# Preprocessor defs
* TETHER_STATIC : Define this before including tether if you are using tether 
	from a static library (.lib or .a)

# Build instructions

First, download these dependencies
- python3
- CMake
- Some C++ compiler (msvc, gcc, MinGW...)

And if you are using `TETHER_RENDERING_VULKAN`:
- Vulkan SDK (from the vulkan-sdk package or https://vulkan.lunarg.com/sdk/home)

Then run cmake in the root repo directory and build the generated files using the chosen CMake generator.
