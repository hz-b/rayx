#include "VulkanTracer.h"



VulkanTracer::VulkanTracer() {
	bufferSizes.resize(3);
	buffers.resize(3);
	bufferMemories.resize(3);
	//beamline.resize(0);

}

VulkanTracer::~VulkanTracer() {

}


//	This function creates a debug messenger
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//	This function destroys the debug messenger
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

/*	this function is used to start the tracer
*/
void VulkanTracer::run()
{
	// At first, the amount of rays is set. This is a placeholder, as the rays won't be generated in this tracer in the future
	setRayAmount();
	std::cout << "Amount of Rays: " << rayAmount << std::endl;
	// generate the rays used by the tracer
	//generateRays();
	//the sizes of the input and output buffers are set. The buffers need to be the size rayamount * 8* the size of a double
	//(a ray consists of 6 values in double precision, x,y,z for the position and x*, y*, z* for the direction. 8 values instead of 6 are used, because the shader size of the buffer needs to be multiples of 16 bit)
	bufferSizes[0] = (uint64_t)rayAmount * RAY_DOUBLE_AMOUNT * sizeof(double);
	bufferSizes[1] = (uint64_t)rayAmount * 4* sizeof(double);
	bufferSizes[2] = beamline.size() * QUADRIC_DOUBLE_AMOUNT * sizeof(double);
	for (int i = 0; i<bufferSizes.size(); i++){
		std::cout << "bufferSizes["<<i<<"]: " << bufferSizes[i] << std::endl;
	}
	//vulkan is initialized
	initVulkan();
	mainLoop();
}

//function for initializing vulkan
void VulkanTracer::initVulkan()
{
	//a vulkan instance is created
	createInstance();

	setupDebugMessenger();

	//physical device for computation is chosen
	pickPhysicalDevice();

	//a logical device is created
	//it is needed to communicate with the physical device
	createLogicalDevice();

	//creates buffers to transfer data to and from the shader
	createBuffers();
	fillInputBuffer();
	fillQuadricBuffer();
	//creates the descriptors used to bind the buffer to shader access points (bindings)
	createDescriptorSetLayout();
	
	createDescriptorSet();

	//a compute pipeline needs to be created
	//the descriptors created earlier will be submitted here
	createComputePipeline();
	createCommandBuffer();
}
void VulkanTracer::mainLoop()
{
	runCommandBuffer();
	//readDataFromOutputBuffer();
}

void VulkanTracer::cleanup()
{
	
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	vkDestroyPipeline(device, pipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	for(int i=0; i<buffers.size();i++){
		vkDestroyBuffer(device, buffers[i], nullptr);
		vkFreeMemory(device, bufferMemories[i], nullptr);
	}
	vkDestroyShaderModule(device, computeShaderModule, nullptr);
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
}
void VulkanTracer::createInstance()
{

	//validation layers are used for debugging
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	//Add description for instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Ray";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 154);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 154);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	//pointer to description with layer count
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = 0;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	//create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("failed to create instance!");
}
void VulkanTracer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}
void VulkanTracer::setupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug messenger!");
	}
}
std::vector<const char *> VulkanTracer::getRequiredExtensions()
{
	std::vector<const char *> extensions;

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanTracer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

//get vector with available layers
bool VulkanTracer::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//check all validation layers
	for (const char *layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
			return false;
	}
	return true;
}

//physical device for computation is chosen
void VulkanTracer::pickPhysicalDevice()
{

	//search for devices
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan Support!");

	//create vector of devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	//search for suitable device for computation task
	for (const auto &device : devices)
	{
		if (isDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}
	//error if no device is found
	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("failed to find suitable GPU!");
	}

	//pick fastest device
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto &device : devices)
	{
		candidates.insert(std::make_pair(rateDevice(device), device));
	}
	candidates.rbegin()->first > 0 ? physicalDevice = candidates.rbegin()->second : throw std::runtime_error("failed to find a suitable GPU!");
}

//checks if given device is suitable for computation
//can be extended later if more specific features are needed
bool VulkanTracer::isDeviceSuitable(VkPhysicalDevice device)
{

	//get device properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	std::cout << "Found GPU:" << deviceProperties.deviceName << std::endl;

	//get device features
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.hasvalue;
}

//rates devices
int VulkanTracer::rateDevice(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	int score = 1;
	//discrete GPUs are usually faster and get a bonus
	//can be extended to choose the best discrete gpu if multiple are available
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;
	return score;
}

VulkanTracer::QueueFamilyIndices VulkanTracer::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	indices.hasvalue = 0;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto &queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			indices.computeFamily = i;
			indices.hasvalue = 1;
		}
		if (indices.isComplete())
			break;
		i++;
	}
	QueueFamily = indices;
	return indices;
}

//creates a logical device to communicate with the physical device
void VulkanTracer::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	//create info about the device queues
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.computeFamily;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	//create info about the device features
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.shaderFloat64 = true;
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	//enable validation layers if possible
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.computeFamily, 0, &computeQueue);
}

// find memory type with desired properties.
uint32_t VulkanTracer::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	/*
	How does this search work?
	See the documentation of VkPhysicalDeviceMemoryProperties for a detailed description.
	*/
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
	{
		if ((memoryTypeBits & (1 << i)) &&
			((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
			return i;
	}
	return -1;
}

void VulkanTracer::createBuffers(){
    std::cout << "number of buffers: " << buffers.size() << std::endl;
	for(int i=0; i<buffers.size();i++){
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    	std::cout << "size of buffer: " << bufferSizes[i] << std::endl;
		bufferCreateInfo.size = bufferSizes[i];
		bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; // buffer is used as a storage buffer.
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;	 // buffer is exclusive to a single queue family at a time.
		VK_CHECK_RESULT(vkCreateBuffer(device, &bufferCreateInfo, NULL, &buffers[i]));
		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(device, buffers[i], &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size; // specify required memory.
		allocateInfo.memoryTypeIndex = findMemoryType(
			memoryRequirements.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device, &allocateInfo, NULL, &(bufferMemories[i]))); // allocate memory on device.
		
		// Now associate that allocated memory with the buffer. With that, the buffer is backed by actual memory.
		VK_CHECK_RESULT(vkBindBufferMemory(device, buffers[i], bufferMemories[i], 0));
	}
	
    std::cout << "Buffers created!" << std::endl;
}

//the input buffer is filled with the ray data
void VulkanTracer::fillInputBuffer()
{
	//data is copied to the buffer
	void *data;
	vkMapMemory(device, bufferMemories[0], 0, bufferSizes[0], 0, &data);
	memcpy(data, rayVector.data(), bufferSizes[0]);
	vkUnmapMemory(device, bufferMemories[0]);
}
//the quad buffer is filled with the quadric data
void VulkanTracer::fillQuadricBuffer()
{
    std::cout << "fill quadric buffer" << std::endl;
	//data is copied to the buffer
	void *data;
	vkMapMemory(device, bufferMemories[2], 0, bufferSizes[2], 0, &data);
    std::cout << "map memory done" << std::endl;
    std::cout << "number of quadrics: " << beamline.size() << std::endl;
    std::cout << "size of quadric buffer: " << bufferSizes[2] << std::endl;
	memcpy(data, beamline.data(), bufferMemories[2]);
    std::cout << "memory copy done" << std::endl;
	vkUnmapMemory(device, bufferMemories[2]);

}
void VulkanTracer::createDescriptorSetLayout()
{
	/*
	Here we specify a descriptor set layout. This allows us to bind our descriptors to
	resources in the shader.
	*/

	/*
	Here we specify a binding of type VK_DESCRIPTOR_TYPE_STORAGE_BUFFER to the binding point
	0. This binds to
		layout(std140, binding = 0) buffer ibuf (input)
	and
		layout(std140, binding = 1) buffer obuf (output)
	in the compute shader.
	*/
	//bindings 0 and 1 are used right now
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding[] = {
		{0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, NULL},
		{1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, NULL},
		{2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, NULL}};

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	//2 bindings are used in this layout
	descriptorSetLayoutCreateInfo.bindingCount = 3;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBinding; //TODO

	// Create the descriptor set layout.
	VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, NULL, &descriptorSetLayout));
}

void VulkanTracer::createDescriptorSet()
{
	/*
	So we will allocate a descriptor set here.
	But we need to first create a descriptor pool to do that.
	*/

	/*
	We use a descriptor set with 2 descriptors: one descriptor for each buffer
	*/
	VkDescriptorPoolSize descriptorPoolSize = {};
	descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorPoolSize.descriptorCount = 3;

	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.maxSets = 1; // we need to allocate one descriptor sets from the pool.
	descriptorPoolCreateInfo.poolSizeCount = 1;
	descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;

	// create descriptor pool.
	VK_CHECK_RESULT(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, NULL, &descriptorPool));

	/*
	With the pool allocated, we can now allocate the descriptor set.
	*/
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool; // pool to allocate from.
	descriptorSetAllocateInfo.descriptorSetCount = 1;		   // allocate a single descriptor set.
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

	// allocate descriptor set.
	VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet));

	
	for(int i=0; i<buffers.size();i++){
	
		//specify which buffer to use: input buffer
		VkDescriptorBufferInfo descriptorBufferInfo = {};
		descriptorBufferInfo.buffer = buffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = bufferSizes[i];

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = descriptorSet;							   // write to this descriptor set.
		writeDescriptorSet.dstBinding = i;									   // write to the ist binding.
		writeDescriptorSet.descriptorCount = 1;								   // update a single descriptor.
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // storage buffer.
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

		// perform the update of the descriptor set.
		vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, NULL);
	}
}

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
uint32_t *VulkanTracer::readFile(uint32_t &length, const char *filename)
{

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		printf("Could not find or open file: %s\n", filename);
	}

	// get file size.
	fseek(fp, 0, SEEK_END);
	long filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	long filesizepadded = long(ceil(filesize / 4.0)) * 4;

	// read file contents.
	char *str = new char[filesizepadded];
	fread(str, filesize, sizeof(char), fp);
	fclose(fp);

	// data padding.
	for (int i = filesize; i < filesizepadded; i++)
	{
		str[i] = 0;
	}

	length = filesizepadded;
	return (uint32_t *)str;
}

void VulkanTracer::createComputePipeline()
{
	/*
	We create a compute pipeline here.
	*/

	/*
	Create a shader module. A shader module basically just encapsulates some shader code.
	*/
	uint32_t filelength;
	// the code in comp.spv was created by running the command:
	// glslangValidator.exe -V shader.comp
	uint32_t *code = readFile(filelength, "comp.spv");
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pCode = code;
	createInfo.codeSize = filelength;

	VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, NULL, &computeShaderModule));
	delete[] code;

	/*
	Now let us actually create the compute pipeline.
	It only consists of a single stage with a compute shader.
	So first we specify the compute shader stage, and it's entry point(main).
	*/
	VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
	shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStageCreateInfo.module = computeShaderModule;
	shaderStageCreateInfo.pName = "main";

	/*
	The pipeline layout allows the pipeline to access descriptor sets.
	So we just specify the descriptor set layout we created earlier.
	*/
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, NULL, &pipelineLayout));

	VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stage = shaderStageCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout;

	/*
	Now, we finally create the compute pipeline.
	*/
	VK_CHECK_RESULT(vkCreateComputePipelines(
		device, VK_NULL_HANDLE,
		1, &pipelineCreateInfo,
		NULL, &pipeline));
}

void VulkanTracer::createCommandBuffer()
{
	/*
	We are getting closer to the end. In order to send commands to the device(GPU),
	we must first record commands into a command buffer.
	To allocate a command buffer, we must first create a command pool. So let us do that.
	*/
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = 0;
	// the queue family of this command pool. All command buffers allocated from this command pool,
	// must be submitted to queues of this family ONLY.
	commandPoolCreateInfo.queueFamilyIndex = QueueFamily.computeFamily;
	VK_CHECK_RESULT(vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool));

	/*
	Now allocate a command buffer from the command pool.
	*/
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool; // specify the command pool to allocate from.
	// if the command buffer is primary, it can be directly submitted to queues.
	// A secondary buffer has to be called from some primary command buffer, and cannot be directly
	// submitted to a queue. To keep things simple, we use a primary command buffer.
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;											   // allocate a single command buffer.
	VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer)); // allocate command buffer.

	/*
	Now we shall start recording commands into the newly allocated command buffer.
	*/
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;	  // the buffer is only submitted and used once in this application.
	VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo)); // start recording commands.

	/*
	We need to bind a pipeline, AND a descriptor set before we dispatch.
	The validation layer will NOT give warnings if you forget these, so be very careful not to forget them.
	*/
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
	//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSets[1], 0, NULL);
	/*
	Calling vkCmdDispatch basically starts the compute pipeline, and executes the compute shader.
	The number of workgroups is specified in the arguments.
	If you are already familiar with compute shaders from OpenGL, this should be nothing new to you.
	*/
	vkCmdDispatch(commandBuffer, (uint32_t)ceil(rayAmount / float(WORKGROUP_SIZE)), 1, 1);

	VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer)); // end recording commands.
}

void VulkanTracer::runCommandBuffer()
{
	/*
	Now we shall finally submit the recorded command buffer to a queue.
	*/

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;			 // submit a single command buffer
	submitInfo.pCommandBuffers = &commandBuffer; // the command buffer to submit.

	/*
		We create a fence.
	*/
	VkFence fence;
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = 0;
	VK_CHECK_RESULT(vkCreateFence(device, &fenceCreateInfo, NULL, &fence));

	/*
	We submit the command buffer on the queue, at the same time giving a fence.
	*/
	VK_CHECK_RESULT(vkQueueSubmit(computeQueue, 1, &submitInfo, fence));
	/*
	The command will not have finished executing until the fence is signalled.
	So we wait here.
	We will directly after this read our buffer from the GPU,
	and we will not be sure that the command has finished executing unless we wait for the fence.
	Hence, we use a fence here.
	*/
	VK_CHECK_RESULT(vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000));

	vkDestroyFence(device, fence, NULL);
}
void VulkanTracer::setRayAmount(uint32_t inputRayAmount)
{
	rayAmount = inputRayAmount;
}
void VulkanTracer::setRayAmount()
{
	rayAmount = rayVector.size();
}

//this function reads the data from the output buffer after the shader has finished the computation
void VulkanTracer::readDataFromOutputBuffer()
{
	void *mappedMemory = NULL;
	// Map the buffer memory, so that we can read from it on the CPU.
	vkMapMemory(device, bufferMemories[1], 0, bufferSizes[1], 0, &mappedMemory);
	double *pMappedMemory = (double *)mappedMemory;
	std::vector<double> data;
	//reserve enough data for all the rays
	data.reserve((uint64_t)rayAmount * RAY_DOUBLE_AMOUNT);
	for (int i = 0; i < rayAmount; i++)
	{
		data.emplace_back(pMappedMemory[i]);
	}
}
std::vector<double> VulkanTracer::getRays(){
	
    std::cout << "getRays: rayAmount: "<< rayAmount  << std::endl;
    std::cout << "getRays: bufferSizes[1]: "<< bufferSizes[1]  << std::endl;
	void *mappedMemory = NULL;
	// Map the buffer memory, so that we can read from it on the CPU.
	vkMapMemory(device, bufferMemories[1], 0, bufferSizes[1], 0, &mappedMemory);
    std::cout << "mapping memory done"  << std::endl;
	double *pMappedMemory = (double *)mappedMemory;
	std::vector<double> data;
	//reserve enough data for all the rays
    std::cout << "reserving memory"  << std::endl;
	data.reserve((uint64_t)rayAmount * 4);
	for (int i = 0; i < rayAmount*4; i++)
	{
		data.emplace_back(pMappedMemory[i]);
	}
	return data;
}
//placeholder function to generate simple rays to test the data transfer
void VulkanTracer::generateRays()
{
	rayVector.reserve(rayAmount);
	for (int i = 0; i < rayAmount; i++)
	{
		Ray tempRay(6 * i + 1, 6 * i + 2, 6 * i + 3, 6 * i + 4, 6 * i + 5, 6 * i + 6, 1);
		rayVector.emplace_back(tempRay);
	}
}
void VulkanTracer::addRay(double xpos, double ypos, double zpos, double xdir, double ydir, double zdir, double weight){
	Ray newRay(xpos, ypos, zpos, xdir, ydir, zdir, weight);
	rayVector.emplace_back(newRay);
}

//adds quad to beamline
void VulkanTracer::addBeamLineObject(std::vector<double> inQuadric, std::vector<double> inputInMatrix, std::vector<double> inputOutMatrix){
	assert(inQuadric.size() == 16 && inputInMatrix.size() == 16 && inputOutMatrix.size() == 16);
	//beamline.resize(beamline.size()+1);
	Quadric newQuadric(inQuadric, inputInMatrix, inputOutMatrix);
	beamline.push_back(newQuadric);
    std::cout << "beamline size: "<< beamline.size()  << std::endl;
}

//is not used anymore
int VulkanTracer::main()
{
	VulkanTracer app;

	try
	{
		app.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		std::cout << "finished VulkanTracer failure" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "finished VulkanTracer2" << std::endl;
	return EXIT_SUCCESS;
}
