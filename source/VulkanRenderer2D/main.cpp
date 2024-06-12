// VulkanRenderer2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "D2DEngine.h"

int main()
{
	D2D::D2DEngine engine{};
	engine.Run();

	return EXIT_SUCCESS;
}
