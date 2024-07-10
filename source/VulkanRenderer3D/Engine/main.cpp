#include "D3DEngine.h"

void load()
{
	//std::cout << "wow \n";
}

int main()
{
	D3D::D3DEngine engine{};
	engine.Run(load);

	return EXIT_SUCCESS;
} 
	