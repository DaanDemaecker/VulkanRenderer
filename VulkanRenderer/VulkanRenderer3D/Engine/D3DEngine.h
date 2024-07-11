// D3DEngine.h
// This class will act as the main engine containing the game loop

#ifndef D3DEngineIncluded
#define D3DEngineIncluded

//Standard library includes
#include <functional>

namespace D3D
{
	class D3DEngine final
	{
	public:
		// Constructor
		D3DEngine();

		// Destructor
		~D3DEngine();

		// Function that will run the engine
		// Parameters:
		//     -load: function given that will be executed before game loop, used for loading in objects etc.
		void Run(const std::function<void()>& load);

		D3DEngine(const D3DEngine& other) = delete;
		D3DEngine(D3DEngine&& other) = delete;
		D3DEngine& operator=(const D3DEngine& other) = delete;
		D3DEngine& operator=(D3DEngine&& other) = delete;
	};
}

#endif // !D3DEngineIncluded