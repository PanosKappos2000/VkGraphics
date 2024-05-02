#include <iostream>
#include "EngineCore/VulkanCore.h"

int main()
{
	VulkanTriangle* app = new VulkanTriangle;
	app->RunTriangle();
	delete app;
}