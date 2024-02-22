#include "D3DEngine.h"
#include "VulkanRenderer3D.h"

void load()
{
	//std::cout << "wow \n";
}

int main()
{
	const uint32_t windowWidth = 800;
	const uint32_t windowHeight = 600;

	D3D::D3DEngine engine(static_cast<int>(windowWidth), static_cast<int>(windowHeight));
	engine.Run(load);

	return EXIT_SUCCESS;
} 