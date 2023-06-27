#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <optional>
#include <memory>
#include <chrono>

#include "Structs.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // NDEBUG
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::string MODEL_PATH = "../Resources/Models/viking_room.obj";
const std::string Texture_PATH = "../Resources/Images/viking_room.png";
