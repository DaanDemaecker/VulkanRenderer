// D3DEngine.h
// This class will act as the main engine containing the game loop

#ifndef D3DEngineIncluded
#define D3DEngineIncluded

//Standard library includes
#include <functional>

namespace DDM3
{
	class DDM3Engine final
	{
	public:
		// Constructor
		DDM3Engine();

		// Destructor
		~DDM3Engine();

		// Function that will run the engine
		// Parameters:
		//     -load: function given that will be executed before game loop, used for loading in objects etc.
		void Run(const std::function<void()>& load);

		DDM3Engine(const DDM3Engine& other) = delete;
		DDM3Engine(DDM3Engine&& other) = delete;
		DDM3Engine& operator=(const DDM3Engine& other) = delete;
		DDM3Engine& operator=(DDM3Engine&& other) = delete;
	};
}

#endif // !D3DEngineIncluded