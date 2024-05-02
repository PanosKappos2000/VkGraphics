#include "VulkanInstance.h"

/****************************************************************************************************
* Function Argument 1: Used to access the required glfw extensions and give them to the info struct *
*****************************************************************************************************/
void VulkanInstanceHandle::CreateVulkanInstance(const GlfwWindowHandle& window)
{
    //Gives some extra information to the application
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    /* Initializing Create info struct for vulkan instance */
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    //Passing the application info stuct to the instance
    createInfo.pApplicationInfo = &appInfo;
    //Passing the required extensions for glfw to the instance
    createInfo.enabledExtensionCount = window.GetExtensionCount();
    createInfo.ppEnabledExtensionNames = window.GetExtensionNames();
    //Setting up Validation layers
    createInfo.enabledLayerCount = 0;
    /* Create info struct for vulkan instance complete */

    /* Checking for available extensions */
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    uint32_t currentExtension = 1;
    for (const VkExtensionProperties& extension : extensions)
    {
        std::cout << "-Extension " << currentExtension << " : " << extension.extensionName << '\n';
        ++currentExtension;
    }
    /* Available extension check complete */

    //Creating the vulkan instance and checking if its creation was succesful
    VkResult instanceResult = vkCreateInstance(&createInfo, nullptr, &vk_instance);
    if (instanceResult != VK_SUCCESS)
    {
        __debugbreak();
    }
}

void VulkanInstanceHandle::Cleanup()
{
    vkDestroyInstance(vk_instance, nullptr);
}