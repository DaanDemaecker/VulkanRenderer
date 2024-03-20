#include "D3DEngine.h"
#include "VulkanRenderer3D.h"

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
	