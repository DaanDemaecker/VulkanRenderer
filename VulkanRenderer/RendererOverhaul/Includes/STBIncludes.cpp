// STBIncludes.cpp

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#ifdef NDEBUG
	#ifndef STBI_NO_FAILURE_STRINGS
		#define STBI_NO_FAILURE_STRINGS
	#endif // !STBI_NO_FAILURE_STRINGS
#else
	#ifndef STBI_FAILURE_USERMSG 
		#define STBI_FAILURE_USERMSG 
	#endif // !STBI_FAILURE_USERMSG 
#endif // NDEBUG


#include "STBIncludes.h"