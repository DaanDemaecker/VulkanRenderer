// VulkanRenderer2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Window.h"
#include "VulkanRenderer2D.h"

int main()
{
    D2D::Window::GetInstance();
    D2D::VulkanRenderer2D::GetInstance();
}
