// DirectionalLightObject.h
// This class will hold all the info, buffers and memory for a single directional light

#ifndef DirectionalLightObjectIncluded
#define DirectionalLightObjectIncluded

// File includes
#include "GLFWIncludes.h"

namespace D3D
{
	class DirectionalLightObject
	{
	public:
		DirectionalLightObject() = default;
		~DirectionalLightObject() = default;

		DirectionalLightObject(DirectionalLightObject& other) = delete;
		DirectionalLightObject(DirectionalLightObject&& other) = delete;

		DirectionalLightObject& operator=(DirectionalLightObject& other) = delete;
		DirectionalLightObject& operator=(DirectionalLightObject&& other) = delete;

	private:

	};
}


#endif // !DirectionalLightObjectIncluded