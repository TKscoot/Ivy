#pragma once
#if defined _WIN32
	// OpenGL on Windows needs Windows.h
	#include <Windows.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <array>
#include <unordered_map>
#include <fstream>
#include <typeindex>
#include <filesystem>
