// VulkanRenderer2D.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ConfigManager.h"

int main()
{
    std::cout << D2D::ConfigManager::GetInstance().GetString("ApplicationName") << "\n";
}
