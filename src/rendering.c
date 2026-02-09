#include "core.h"

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "containers.h"
#include "io.h"
#include "config.h"
#include "math.h"
#include "rendering.h"

/*	Things to test:
	1. Put vertex-, index- and uniform buffer in one VkBuffer object for better cache coherency.
	2. Put multiple vertex-, index- and uniform buffers into the same VkBuffer object.
*/

/**************************************************************************************************/
/*	Vulkan  */

typedef struct glvec2 {
	float x, y;
} glvec2;

typedef struct glvec3 {
	float x, y, z;
} glvec3;

typedef struct glvec4 {
	float x, y, z, w;
} glvec4;

typedef struct vertex {
	glvec3 pos;
	glvec3 color;
} vertex;

typedef struct mesh_uniform_data {
	mat4 mvp;
} mesh_uniform_data;

const vertex mesh_vertices[4] = {
	{{-0.5f, 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const u32 mesh_indices[6] = {
	0, 1, 2, 2, 3, 0
};

#define VKASSERT(call, message) if ((call) != VK_SUCCESS) throw_exception("%s \"file://%s\": %i", \
	message, __FILE__, __LINE__);

typedef struct vk_device {
	VkPhysicalDevice physical_device;
	VkPhysicalDeviceFeatures enabled_features;
	u32 graphics_family;
	u32 compute_family;
	u32 transfer_family;
	u32 graphics_queue_count;
	u32 compute_queue_count;
	u32 transfer_queue_count;
	VkDevice handle;
	VkQueue graphics_queue;
	VkQueue compute_queue;
	VkQueue transfer_queue;
	VkCommandPool graphics_pool;
	VkCommandPool compute_pool;
	VkCommandPool transfer_pool;
	VkCommandBuffer render_buffers[MAX_PARALLEL_FRAMES];
	VkSemaphore image_semaphores[MAX_PARALLEL_FRAMES]; /* should probably go into surface */
	VkSemaphore done_semaphores[MAX_PARALLEL_FRAMES];
	VkFence done_fences[MAX_PARALLEL_FRAMES];
} vk_device;

typedef struct vk_window_surface {
	VkSurfaceKHR handle;
	VkSwapchainKHR swapchain;
	VkSurfaceFormatKHR sdr_format;
	VkPresentModeKHR present_mode;
	VkExtent2D extent;
	VkImage* images;
	VkImageView* views;
	VkFramebuffer* framebuffers;
	u32 image_count;
	u32 present_family;
	VkQueue present_queue;
} vk_window_surface;

typedef struct vk_mesh {
	VkBuffer vertex_buffer;
	VkBuffer index_buffer;
	VkDeviceMemory vertex_memory;
	VkDeviceMemory index_memory;
	u32 vertex_count;
	u32 index_count;
	VkBuffer uniform_buffers[MAX_PARALLEL_FRAMES];
	VkDeviceMemory uniform_memory_handles[MAX_PARALLEL_FRAMES];
	void* mapped_uniform_handles[MAX_PARALLEL_FRAMES];
	VkDescriptorSet descriptor_sets[MAX_PARALLEL_FRAMES];
	mat4 model;
} vk_mesh;

static VkInstance instance = VK_NULL_HANDLE;
static u32 frame_idx = 0;

/*	Per GPU  */
static vk_device device;

/*  Per Surface  */
static vk_window_surface surface;

/*  Render Passes  */
VkRenderPass g_default_render_pass;

/*  Material  */
VkDescriptorSetLayout g_default_descriptor_layout;
VkPipelineLayout g_default_layout;
VkPipeline g_default_pipeline;

/*  Mesh  */
VkDescriptorPool g_mesh_descriptor_pool;
static vk_mesh mesh;

/*	View  */
static mat4 view;
static mat4 proj;

static void get_binding_description(VkVertexInputBindingDescription* binding)
{
	binding->binding = 0;
	binding->stride = sizeof(vertex);
	binding->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

static void get_attribute_descriptions(VkVertexInputAttributeDescription* attributes)
{
	attributes[0].location = 0;
	attributes[0].binding = 0;
	attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[0].offset = 0;
	attributes[1].location = 1;
	attributes[1].binding = 0;
	attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributes[1].offset = offsetof(vertex, color);
}

/* Would probably need to know about window system being used. */
static VkInstance create_instance()
{
	VkInstance instance;
	VkApplicationInfo appInfo = { 0 };
	VkInstanceCreateInfo ici = { 0 };

	const char* instance_layers[] = { "VK_LAYER_KHRONOS_validation" };
	const char* instance_extensions[] = { "VK_KHR_surface", VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "SadEngine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "SadEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	ici.pApplicationInfo = &appInfo;
	ici.enabledLayerCount = 1;
	ici.ppEnabledLayerNames = instance_layers;
	ici.enabledExtensionCount = 2;
	ici.ppEnabledExtensionNames = instance_extensions;

	VKASSERT(vkCreateInstance(&ici, NULL, &instance), "vkCreateInstance failed!");
	return instance;
}

static sint check_device_extension_support(VkPhysicalDevice gpu)
{
	VkExtensionProperties* extensions;
	u32 num, i, j, supported_count = 0;
	
	const char* required[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	u32 required_count = 1;


	vkEnumerateDeviceExtensionProperties(gpu, NULL, &num, NULL);
	extensions = calloc(num, sizeof(VkExtensionProperties));

	if (extensions == NULL)
		return 0;

	vkEnumerateDeviceExtensionProperties(gpu, NULL, &num, extensions);

	for (i = 0; i < required_count; i++)
		for (j = 0; j < num; j++)
			if (strcmp(required[i], extensions[j].extensionName) == 0)
				supported_count++;

	free(extensions);
	return supported_count == required_count;
}

static u32 judge_gpu(VkPhysicalDevice gpu)
{
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memory;
	u32 score = 1;

	if (!check_device_extension_support(gpu))
		return 0;

	vkGetPhysicalDeviceProperties(gpu, &properties);
	vkGetPhysicalDeviceFeatures(gpu, &features);
	vkGetPhysicalDeviceMemoryProperties(gpu, &memory);

	switch (properties.deviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			score |= 3u << 30u;
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			score |= 2u << 30u;
			break;
		default:
			score |= 1u << 30u;
			break;
	}

	return score;
}

static VkPhysicalDevice get_physical_device(void)
{
	VkPhysicalDevice physical_devices[16];
	u32 i, num_physical_devices, index, highscore = 0;

	vkEnumeratePhysicalDevices(instance, &num_physical_devices, NULL);
	num_physical_devices = num_physical_devices > 16 ? 16 : num_physical_devices;
	vkEnumeratePhysicalDevices(instance, &num_physical_devices, physical_devices);

	if (num_physical_devices == 0)
		goto error;

	for (i = 0; i < num_physical_devices; i++)
	{
		u32 score = judge_gpu(physical_devices[i]);
		
		if (score > highscore)
		{
			highscore = score;
			index = i;
		}
	}

	if (highscore > 0)
		return physical_devices[index];

error:
	VKASSERT(VK_ERROR_UNKNOWN, "Could not find physical device!");
	return VK_NULL_HANDLE;
}

static void get_queue_config(vk_device* device)
{
	VkQueueFamilyProperties properties[16];
	u32 property_count, i;
	u32 graphics_highscore = 0;
	u32 compute_highscore = 0;
	u32 transfer_highscore = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(device->physical_device, &property_count, NULL);
	property_count = property_count > 16 ? 16 : property_count;
	vkGetPhysicalDeviceQueueFamilyProperties(device->physical_device, &property_count, properties);

	for (i = 0; i < property_count; i++)
	{
		u32 flags = properties[i].queueFlags;
		u32 graphics_score = (flags & VK_QUEUE_GRAPHICS_BIT) != 0;
		u32 compute_score = (flags & VK_QUEUE_COMPUTE_BIT) != 0;
		u32 transfer_score = (flags & VK_QUEUE_TRANSFER_BIT) != 0;

		graphics_score *= 2 - compute_score;
		compute_score *= 2 - graphics_score;
		transfer_score *= 2 - (graphics_score || compute_score);

		if (graphics_score > graphics_highscore)
		{
			graphics_highscore = graphics_score;
			device->graphics_family = i;
			device->graphics_queue_count = properties[i].queueCount;
		}
		if (compute_score > compute_highscore)
		{
			compute_highscore = compute_score;
			device->compute_family = i;
			device->compute_queue_count = properties[i].queueCount;
		}
		if (transfer_score > transfer_highscore)
		{
			transfer_highscore = transfer_score;
			device->transfer_family = i;
			device->transfer_queue_count = properties[i].queueCount;
		}
	}

	if (graphics_highscore == 0 || compute_highscore == 0)
	{
		VKASSERT(VK_ERROR_UNKNOWN, "Your gpu does not have a graphics and/or compute queue??!");
		return;
	}
}

static void fill_queue_create_infos(vk_device* device, VkDeviceQueueCreateInfo* infos, u32* count,
	float* priorities)
{
	infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	infos[0].pQueuePriorities = priorities;
	infos[0].queueFamilyIndex = device->graphics_family;
	infos[0].queueCount = 1;
	*count = 1;

	if (device->compute_family != device->graphics_family)
	{
		int index = (*count)++;
		infos[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		infos[index].pQueuePriorities = priorities;
		infos[index].queueFamilyIndex = device->compute_family;
		infos[index].queueCount = 1;
	}
	else if (device->compute_queue_count > 2)
	{
		infos[0].queueCount++;
	}

	if (device->transfer_family != device->graphics_family &&
		device->transfer_family != device->compute_family)
	{
		int index = (*count)++;
		infos[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		infos[index].pQueuePriorities = priorities;
		infos[index].queueFamilyIndex = device->transfer_family;
		infos[index].queueCount = 1;
	}
	else if (device->graphics_queue_count > 1)
	{
		infos[0].queueCount++;
	}
}

static void get_queue_handles(vk_device* device)
{
	int index = 0;
	/* get graphics queue handle */
	vkGetDeviceQueue(device->handle, device->graphics_family, index++, &device->graphics_queue);

	/* get compute queue handle */
	if (device->compute_family != device->graphics_family)
		vkGetDeviceQueue(device->handle, device->compute_family, 0, &device->compute_queue);

	else if (device->graphics_queue_count > 2)
		vkGetDeviceQueue(device->handle, device->graphics_family, index++, &device->compute_queue);

	else
		device->compute_queue = device->graphics_queue;

	/* get transfer queue handle */
	if (device->transfer_family != device->graphics_family &&
		device->transfer_family != device->compute_family)
		vkGetDeviceQueue(device->handle, device->transfer_family, 0, &device->transfer_queue);

	else if (device->graphics_queue_count > 1)
		vkGetDeviceQueue(device->handle, device->graphics_family, index++, &device->transfer_queue);

	else
		device->transfer_queue = device->graphics_queue;
}

static void create_command_pools(vk_device* device)
{
	VkCommandPoolCreateInfo pool_info = { 0 };
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = device->graphics_family;
	VKASSERT(vkCreateCommandPool(device->handle, &pool_info, NULL, &device->graphics_pool),
		"Failed to create command pool!");

	pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_info.queueFamilyIndex = device->compute_family;
	VKASSERT(vkCreateCommandPool(device->handle, &pool_info, NULL, &device->compute_pool),
		"Failed to create command pool!");

	pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	pool_info.queueFamilyIndex = device->transfer_family;
	VKASSERT(vkCreateCommandPool(device->handle, &pool_info, NULL, &device->transfer_pool),
		"Failed to create command pool!");
}

static void destroy_command_pools(vk_device* device)
{
	vkDestroyCommandPool(device->handle, device->graphics_pool, NULL);
	vkDestroyCommandPool(device->handle, device->compute_pool, NULL);
	vkDestroyCommandPool(device->handle, device->transfer_pool, NULL);
}

static void create_device(vk_device* device)
{
	float queue_priorities[3] = { 1.0f, 1.0f, 1.0f };

	VkDeviceCreateInfo device_info = { 0 };
	VkPhysicalDeviceFeatures device_features;
	VkDeviceQueueCreateInfo queue_infos[3] = { 0 };
	u32 queue_info_count;

	const char* device_layers[] = { "VK_LAYER_KHRONOS_validation" };
	const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	/*  get physical device  */
	device->physical_device = get_physical_device();

	/*  get physical device features  */
	vkGetPhysicalDeviceFeatures(device->physical_device, &device_features);
	device->enabled_features.samplerAnisotropy = device_features.samplerAnisotropy;

	/*  get queue config  */
	get_queue_config(device);
	fill_queue_create_infos(device, queue_infos, &queue_info_count, queue_priorities);

	/*  create device  */
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = queue_info_count;
	device_info.pQueueCreateInfos = queue_infos;
	device_info.enabledLayerCount = 1;
	device_info.ppEnabledLayerNames = device_layers;
	device_info.enabledExtensionCount = 1;
	device_info.ppEnabledExtensionNames = device_extensions;
	device_info.pEnabledFeatures = &device->enabled_features;

	VKASSERT(vkCreateDevice(device->physical_device, &device_info, NULL, &device->handle),
		"Failed to create VkDevice!");

	get_queue_handles(device);
	create_command_pools(device);
}

VkPresentModeKHR get_present_mode(vk_device* device, VkSurfaceKHR surface, u32 max_image_count,
	u32* out_image_count)
{
	VkPresentModeKHR modes[8];
	u32 count, i;
	u32 mailbox = 0;
	u32 immediate = 0;

	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical_device, surface, &count, NULL);
	count = count > 8 ? 8 : count;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical_device, surface, &count, modes);

	if (max_image_count == 0)
		max_image_count = 3;

	for (i = 0; i < count; i++)
	{
		if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			immediate = 1;
		else if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR && max_image_count >= 3)
			mailbox = 1;
	}

	if (g_wndcfg.vsync == 3 && mailbox)
	{
		*out_image_count = 3;
		return VK_PRESENT_MODE_MAILBOX_KHR;
	}
	else if (g_wndcfg.vsync == 0 && immediate)
	{
		*out_image_count = min(max_image_count, 3);
		return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}
	else
	{
		*out_image_count = min(max_image_count, 2);
		return VK_PRESENT_MODE_FIFO_KHR;
	}
}

VkSurfaceFormatKHR get_surface_format(vk_device* device, VkSurfaceKHR surface)
{
	VkSurfaceFormatKHR* formats;
	VkSurfaceFormatKHR sdr = { VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	u32 count, i;

	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &count, NULL);
	formats = calloc(count, sizeof(VkSurfaceFormatKHR));

	if (formats == NULL)
		return sdr;

	vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &count, formats);

	for (i = 0; i < count; i++)
	{
		if (formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			continue;
		else if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
			break;
		else if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
			break;
	}

	if (i == count)
	{
		VKASSERT(VK_ERROR_UNKNOWN, "Failed to find surface format!");
		goto ret;
	}

	sdr = formats[i];
ret:
	free(formats);
	return sdr;
}

void get_present_queue_and_family(vk_device* device, vk_window_surface* surface)
{
	VkBool32 support;
	vkGetPhysicalDeviceSurfaceSupportKHR(device->physical_device, device->graphics_family,
		surface->handle, &support);

	if (support == VK_TRUE)
	{
		surface->present_family = device->graphics_family;
		surface->present_queue = device->graphics_queue;
		return;
	}

	vkGetPhysicalDeviceSurfaceSupportKHR(device->physical_device, device->compute_family,
		surface->handle, &support);

	if (support == VK_TRUE)
	{
		surface->present_family = device->compute_family;
		surface->present_queue = device->compute_queue;
		return;
	}

	VKASSERT(VK_ERROR_UNKNOWN, "Could not find a queue family capable of presenting to surface!");
}

void create_swapchain_image_views(vk_window_surface* surface)
{
	u32 i;

	vkGetSwapchainImagesKHR(device.handle, surface->swapchain, &surface->image_count, NULL);
	surface->images = calloc(surface->image_count, sizeof(VkImage));
	surface->views = calloc(surface->image_count, sizeof(VkImageView));
	surface->framebuffers = calloc(surface->image_count, sizeof(VkFramebuffer));

	if (surface->images == NULL || surface->views == NULL || surface->framebuffers == NULL)
	{
		free(surface->images);
		free(surface->views);
		free(surface->framebuffers);
		VKASSERT(VK_ERROR_UNKNOWN, "Failed to allocate swapchain image handles!");
		return;
	}

	vkGetSwapchainImagesKHR(device.handle, surface->swapchain, &surface->image_count, surface->images);

	for (i = 0; i < surface->image_count; i++)
	{
		VkImageViewCreateInfo view_info = { 0 };
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = surface->images[i];
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = surface->sdr_format.format;
		view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;
		VKASSERT(vkCreateImageView(device.handle, &view_info, NULL, surface->views + i),
			"Failed to create image views!");
	}
}

void create_swapchain(vk_window_surface* surface)
{
	u32 family_indices[2] = { device.graphics_family };
	VkSwapchainCreateInfoKHR swapchain_create_info = { 0 };
	VkSurfaceCapabilitiesKHR caps;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical_device, surface->handle, &caps);
	surface->extent = caps.currentExtent;
	surface->sdr_format = get_surface_format(&device, surface->handle);
	get_present_queue_and_family(&device, surface);
	surface->present_mode = get_present_mode(&device, surface->handle, caps.maxImageCount,
		&surface->image_count);

	swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_create_info.surface = surface->handle;
	swapchain_create_info.minImageCount = surface->image_count;
	swapchain_create_info.imageFormat = surface->sdr_format.format;
	swapchain_create_info.imageColorSpace = surface->sdr_format.colorSpace;
	swapchain_create_info.imageExtent = surface->extent;
	swapchain_create_info.imageArrayLayers = 1;
	swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchain_create_info.queueFamilyIndexCount = 1;
	swapchain_create_info.pQueueFamilyIndices = family_indices;
	swapchain_create_info.preTransform = caps.currentTransform;
	swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_create_info.presentMode = surface->present_mode;
	swapchain_create_info.clipped = VK_TRUE;
	swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

	if (surface->present_family != device.graphics_family)
	{
		swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_create_info.queueFamilyIndexCount = 2;
		family_indices[1] = surface->present_family;
	}

	VKASSERT(vkCreateSwapchainKHR(device.handle, &swapchain_create_info, NULL, &surface->swapchain),
		"Failed to create swapchain!");

	create_swapchain_image_views(surface);
}

void destroy_swapchain(vk_window_surface* surface)
{
	u32 i;
	for (i = 0; i < surface->image_count; i++)
		vkDestroyImageView(device.handle, surface->views[i], NULL);

	vkDestroySwapchainKHR(device.handle, surface->swapchain, NULL);
}

void create_window_surface(window* window)
{
	VkWin32SurfaceCreateInfoKHR surface_create_info = { 0 };

	surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_create_info.hwnd = window->handle;
	surface_create_info.hinstance = GetModuleHandle(NULL);

	VKASSERT(vkCreateWin32SurfaceKHR(instance, &surface_create_info, NULL, &surface.handle),
		"Failed to create Win32 surface!");

	create_swapchain(&surface);
}

void destroy_window_surface()
{
	destroy_swapchain(&surface);
	vkDestroySurfaceKHR(instance, surface.handle, NULL);
}

VkShaderModule create_shader_module(void* code, size_t size)
{
	VkShaderModule mod;
	VkShaderModuleCreateInfo shader_info = { 0 };
	shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_info.codeSize = size;
	shader_info.pCode = (u32*)code;

	VKASSERT(vkCreateShaderModule(device.handle, &shader_info, NULL, &mod),
		"Failed to create shader module!");
	return mod;
}

VkShaderModule load_shader_module(const char* path)
{
	VkShaderModule shader;
	void* code;
	size_t size;

	code = (u32*)load_file(path, &size);
	shader = create_shader_module(code, size);
	free(code);

	return shader;
}

void create_render_pass()
{
	VkAttachmentDescription color_attachment = { 0 };
	VkAttachmentReference color_attachment_ref = { 0 };
	VkSubpassDependency color_dependency = { 0 };
	VkSubpassDescription subpass = { 0 };
	VkRenderPassCreateInfo renderpass_info = { 0 };

	color_attachment.format = surface.sdr_format.format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	color_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	color_dependency.dstSubpass = 0;
	color_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	color_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	color_dependency.srcAccessMask = 0;
	color_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = NULL;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pResolveAttachments = NULL;
	subpass.pDepthStencilAttachment = NULL;
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = NULL;

	renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_info.attachmentCount = 1;
	renderpass_info.pAttachments = &color_attachment;
	renderpass_info.subpassCount = 1;
	renderpass_info.pSubpasses = &subpass;
	renderpass_info.dependencyCount = 1;
	renderpass_info.pDependencies = &color_dependency;
	VKASSERT(vkCreateRenderPass(device.handle, &renderpass_info, NULL, &g_default_render_pass),
		"Failed to create render pass!");
}

void destroy_render_pass()
{
	vkDestroyRenderPass(device.handle, g_default_render_pass, NULL);
}

void create_descriptor_set_layout()
{
	VkDescriptorSetLayoutBinding binding = { 0 };
	VkDescriptorSetLayoutCreateInfo dsl_info = { 0 };

	binding.binding = 0;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	binding.pImmutableSamplers = NULL;

	dsl_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	dsl_info.bindingCount = 1;
	dsl_info.pBindings = &binding;

	VKASSERT(vkCreateDescriptorSetLayout(device.handle, &dsl_info, NULL, &g_default_descriptor_layout),
		"Failed to create descriptor set layout!");
}

void create_graphics_pipelines()
{
	VkShaderModule vertex_shader = load_shader_module("shaders/spv/ubo_mesh.vert.spv");
	VkShaderModule fragment_shader = load_shader_module("shaders/spv/triangle.frag.spv");

	VkVertexInputBindingDescription vertex_bindings[1];
	VkVertexInputAttributeDescription vertex_attributes[2];
	VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineShaderStageCreateInfo shader_stage_infos[2] = { 0 };
	VkPipelineDynamicStateCreateInfo dynamic_info = { 0 };
	VkPipelineVertexInputStateCreateInfo vertex_input_info = { 0 };
	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = { 0 };
	VkPipelineViewportStateCreateInfo viewport_info = { 0 };
	VkPipelineRasterizationStateCreateInfo rasterizer_info = { 0 };
	VkPipelineMultisampleStateCreateInfo multisampling_info = { 0 };
	VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
	VkPipelineColorBlendStateCreateInfo color_blend_info = { 0 };
	VkGraphicsPipelineCreateInfo pipeline_info = { 0 };

	/*  vertex shader */
	shader_stage_infos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_infos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shader_stage_infos[0].module = vertex_shader;
	shader_stage_infos[0].pName = "main";

	/*  fragment shader stage  */
	shader_stage_infos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_infos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shader_stage_infos[1].module = fragment_shader;
	shader_stage_infos[1].pName = "main";

	dynamic_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_info.dynamicStateCount = 2;
	dynamic_info.pDynamicStates = dynamic_states;

	get_binding_description(vertex_bindings);
	get_attribute_descriptions(vertex_attributes);

	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 1;
	vertex_input_info.pVertexBindingDescriptions = vertex_bindings;
	vertex_input_info.vertexAttributeDescriptionCount = 2;
	vertex_input_info.pVertexAttributeDescriptions = vertex_attributes;

	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = VK_FALSE;

	viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_info.viewportCount = 1;
	viewport_info.scissorCount = 1;

	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = VK_FALSE;
	rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	rasterizer_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer_info.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer_info.depthBiasEnable = VK_FALSE;
	rasterizer_info.depthBiasConstantFactor = 0.0f;
	rasterizer_info.depthBiasClamp = 0.0f;
	rasterizer_info.depthBiasSlopeFactor = 0.0f;
	rasterizer_info.lineWidth = 1.0f;

	multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling_info.sampleShadingEnable = VK_FALSE;
	multisampling_info.minSampleShading = 1.0f;
	multisampling_info.pSampleMask = NULL;
	multisampling_info.alphaToCoverageEnable = VK_FALSE;
	multisampling_info.alphaToOneEnable = VK_FALSE;

	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_info.logicOpEnable = VK_FALSE;
	color_blend_info.logicOp = VK_LOGIC_OP_COPY;
	color_blend_info.attachmentCount = 1;
	color_blend_info.pAttachments = &color_blend_attachment;
	color_blend_info.blendConstants[0] = 0.0f;
	color_blend_info.blendConstants[1] = 0.0f;
	color_blend_info.blendConstants[2] = 0.0f;
	color_blend_info.blendConstants[3] = 0.0f;

	{
		VkPipelineLayoutCreateInfo layout_info = { 0 };
		layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layout_info.setLayoutCount = 1;
		layout_info.pSetLayouts = &g_default_descriptor_layout;
		layout_info.pushConstantRangeCount = 0;
		layout_info.pPushConstantRanges = NULL;
		VKASSERT(vkCreatePipelineLayout(device.handle, &layout_info, NULL, &g_default_layout),
			"Failed to create pipeline layout!");
	}

	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.stageCount = 2;
	pipeline_info.pStages = shader_stage_infos;
	pipeline_info.pVertexInputState = &vertex_input_info;
	pipeline_info.pInputAssemblyState = &input_assembly_info;
	pipeline_info.pTessellationState = NULL;
	pipeline_info.pViewportState = &viewport_info;
	pipeline_info.pRasterizationState = &rasterizer_info;
	pipeline_info.pMultisampleState = &multisampling_info;
	pipeline_info.pDepthStencilState = NULL;
	pipeline_info.pColorBlendState = &color_blend_info;
	pipeline_info.pDynamicState = &dynamic_info;
	pipeline_info.layout = g_default_layout;
	pipeline_info.renderPass = g_default_render_pass;
	pipeline_info.subpass = 0;
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	pipeline_info.basePipelineIndex = U32_MAX;

	VKASSERT(vkCreateGraphicsPipelines(device.handle, VK_NULL_HANDLE, 1, &pipeline_info, NULL,
		&g_default_pipeline), "Failed to create graphics pipeline!");

	vkDestroyShaderModule(device.handle, vertex_shader, NULL);
	vkDestroyShaderModule(device.handle, fragment_shader, NULL);
}

void destroy_graphics_pipelines()
{
	vkDestroyPipelineLayout(device.handle, g_default_layout, NULL);
	vkDestroyPipeline(device.handle, g_default_pipeline, NULL);
}

void create_framebuffers()
{
	u32 i;
	for (i = 0; i < surface.image_count; i++)
	{
		VkFramebufferCreateInfo framebuffer_info = { 0 };
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.renderPass = g_default_render_pass;
		framebuffer_info.attachmentCount = 1;
		framebuffer_info.pAttachments = surface.views + i;
		framebuffer_info.width = surface.extent.width;
		framebuffer_info.height = surface.extent.height;
		framebuffer_info.layers = 1;
		VKASSERT(vkCreateFramebuffer(device.handle, &framebuffer_info, NULL, surface.framebuffers + i),
			"Failed to create framebuffer!");
	}
}

void destroy_framebuffers()
{
	u32 i;
	for (i = 0; i < surface.image_count; i++)
		vkDestroyFramebuffer(device.handle, surface.framebuffers[i], NULL);
}

void recreate_swapchain()
{
	vkDeviceWaitIdle(device.handle);

	destroy_framebuffers();
	destroy_swapchain(&surface);

	create_swapchain(&surface);
	create_framebuffers();
}

void create_command_buffers()
{
	VkCommandBufferAllocateInfo alloc_info = { 0 };
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.commandPool = device.graphics_pool;
	alloc_info.commandBufferCount = g_rndcfg.parallel_frames;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkAllocateCommandBuffers(device.handle, &alloc_info, device.render_buffers);
}

void create_sync_objects()
{
	VkSemaphoreCreateInfo sem_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fence_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	u32 i;

	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (i = 0; i < g_rndcfg.parallel_frames; i++)
	{
		if (vkCreateSemaphore(device.handle, &sem_info, NULL, device.image_semaphores + i) != VK_SUCCESS ||
			vkCreateSemaphore(device.handle, &sem_info, NULL, device.done_semaphores + i) != VK_SUCCESS||
			vkCreateFence(device.handle, &fence_info, NULL, device.done_fences + i) != VK_SUCCESS)
			VKASSERT(VK_ERROR_UNKNOWN, "Failed to create synchronization objects!");
	}
}

void destroy_sync_objects()
{
	u32 i;

	vkDeviceWaitIdle(device.handle);

	for (i = 0; i < g_rndcfg.parallel_frames; i++)
	{
		vkDestroySemaphore(device.handle, device.image_semaphores[i], NULL);
		vkDestroySemaphore(device.handle, device.done_semaphores[i], NULL);
		vkDestroyFence(device.handle, device.done_fences[i], NULL);
	}
}

u32 find_memory_type(u32 filter, VkMemoryPropertyFlags flags)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	u32 i;

	vkGetPhysicalDeviceMemoryProperties(device.physical_device, &memory_properties);

	for (i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if (filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & flags) == flags)
			return i;
	}

	VKASSERT(VK_ERROR_UNKNOWN, "Failed to find memory type!");
	return VK_MAX_MEMORY_TYPES;
}

void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	u32 family_count, u32* families, VkBuffer* out_buffer, VkDeviceMemory* out_memory)
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	VkMemoryAllocateInfo alloc_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	VkMemoryRequirements memory_requirements;

	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = family_count > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
	buffer_info.queueFamilyIndexCount = family_count;
	buffer_info.pQueueFamilyIndices = families;

	VKASSERT(vkCreateBuffer(device.handle, &buffer_info, NULL, &buffer),
		"Failed to create buffer!");

	vkGetBufferMemoryRequirements(device.handle, buffer, &memory_requirements);

	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits, properties);

	VKASSERT(vkAllocateMemory(device.handle, &alloc_info, NULL, &memory),
		"Failed to allocate buffer memory!");

	vkBindBufferMemory(device.handle, buffer, memory, 0);
	*out_buffer = buffer;
	*out_memory = memory;
}

void destroy_buffer(VkBuffer buffer, VkDeviceMemory memory)
{
	vkDestroyBuffer(device.handle, buffer, NULL);
	vkFreeMemory(device.handle, memory, NULL);
}

void copy_buffer(VkBuffer dst, VkBuffer src, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo alloc_info = { 0 };
	VkCommandBuffer cmdbuff;
	VkCommandBufferBeginInfo begin_info = { 0 };
	VkBufferCopy region = { 0 };
	VkSubmitInfo submit_info = { 0 };

	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = device.graphics_pool;
	alloc_info.commandBufferCount = 1;

	vkAllocateCommandBuffers(device.handle, &alloc_info, &cmdbuff);

	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdbuff, &begin_info);

	region.srcOffset = 0;
	region.dstOffset = 0;
	region.size = size;

	vkCmdCopyBuffer(cmdbuff, src, dst, 1, &region);
	vkEndCommandBuffer(cmdbuff);

	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &cmdbuff;

	vkQueueSubmit(device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
	vkQueueWaitIdle(device.graphics_queue);

	vkFreeCommandBuffers(device.handle, device.graphics_pool, 1, &cmdbuff);
}

void create_staged_buffer(const void* data, VkDeviceSize size, VkBufferUsageFlags usage,
	VkBuffer* out_buffer, VkDeviceMemory* out_memory)
{
	VkBuffer staging_buffer;
	VkDeviceMemory staging_memory;
	void* mem;

	create_buffer(size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		1, &device.graphics_family,
		&staging_buffer, &staging_memory);

	vkMapMemory(device.handle, staging_memory, 0, size, 0, &mem);
	memcpy(mem, data, (size_t)size);
	vkUnmapMemory(device.handle, staging_memory);

	create_buffer(
		size,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		1, &device.graphics_family,
		out_buffer, out_memory);

	copy_buffer(*out_buffer, staging_buffer, size);

	destroy_buffer(staging_buffer, staging_memory);
}

void create_uniform_buffers()
{
	u32 i;

	for (i = 0; i < MAX_PARALLEL_FRAMES; i++)
	{
		create_buffer(sizeof(mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			1, &device.graphics_family, mesh.uniform_buffers + i, mesh.uniform_memory_handles + i);
		vkMapMemory(device.handle, mesh.uniform_memory_handles[i], 0, sizeof(mat4), 0,
			mesh.mapped_uniform_handles + i);
	}
}

void destroy_uniform_buffers()
{
	u32 i;

	for (i = 0; i < MAX_PARALLEL_FRAMES; i++)
	{
		destroy_buffer(mesh.uniform_buffers[i], mesh.uniform_memory_handles[i]);
	}
}

void update_uniform_buffer(u32 frame_idx)
{
	mesh_uniform_data uniform_data;

	flt fov = flt_tan(90.0 / 2.0);
	flt ar = (flt)surface.extent.width / (flt)surface.extent.height;

	transform t = {
		vec3_set(0.0, 1.0, 0.0),
		quat_from_euler_angles(vec3_set(0.0, 0.0, 45.0)),
		vec3_set(1.0, 1.0, 1.0)
	};

	mat4 model = mat4_model(t);
	mat4 view = mat4_view(vec3_set(0.0, 0.0, 0.0), vec3_set(0.0, 0.0, 0.0));
	mat4 proj = mat4_perspective_vk(90.0, ar, -0.02, 0.0);
	mat4 mvp = mat4_mvp(model, view, proj);

	uniform_data.mvp = mat4_transpose(mvp);

	memcpy(mesh.mapped_uniform_handles[frame_idx], &uniform_data, sizeof(mesh_uniform_data));
}

/*	VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag can be used to allow individual
	descriptor sets to be freed.  */
void create_descriptor_pool()
{
	VkDescriptorPoolCreateInfo pool_info = { 0 };
	VkDescriptorPoolSize pool_size = { 0 };

	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = MAX_PARALLEL_FRAMES;

	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.maxSets = MAX_PARALLEL_FRAMES;
	pool_info.poolSizeCount = 1;
	pool_info.pPoolSizes = &pool_size;

	VKASSERT(vkCreateDescriptorPool(device.handle, &pool_info, NULL, &g_mesh_descriptor_pool),
		"Failed to create descriptor pool!");
}

void create_descriptor_sets()
{
	int i;
	VkDescriptorSetLayout layouts[MAX_PARALLEL_FRAMES] = { g_default_descriptor_layout,
		g_default_descriptor_layout, g_default_descriptor_layout };
	VkDescriptorSetAllocateInfo alloc_info = { 0 };
	VkDescriptorBufferInfo buffer_infos[MAX_PARALLEL_FRAMES] = { 0 };
	VkWriteDescriptorSet write_infos[MAX_PARALLEL_FRAMES] = { 0 };

	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = g_mesh_descriptor_pool;
	alloc_info.descriptorSetCount = MAX_PARALLEL_FRAMES;
	alloc_info.pSetLayouts = layouts;

	VKASSERT(vkAllocateDescriptorSets(device.handle, &alloc_info, mesh.descriptor_sets),
		"Failed to allocate mesh->descriptor_sets!");

	for (i = 0; i < MAX_PARALLEL_FRAMES; i++)
	{
		buffer_infos[i].offset = 0;
		buffer_infos[i].range = sizeof(mesh_uniform_data);
		buffer_infos[i].buffer = mesh.uniform_buffers[i];

		write_infos[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_infos[i].dstSet = mesh.descriptor_sets[i];
		write_infos[i].dstBinding = 0;
		write_infos[i].dstArrayElement = 0;
		write_infos[i].descriptorCount = 1;
		write_infos[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write_infos[i].pImageInfo = NULL;
		write_infos[i].pBufferInfo = buffer_infos + i;
		write_infos[i].pTexelBufferView = NULL;
	}

	vkUpdateDescriptorSets(device.handle, 3, write_infos, 0, NULL);
}

void destroy_descriptor_pool()
{
	vkDestroyDescriptorPool(device.handle, g_mesh_descriptor_pool, NULL);
}

void create_mesh()
{
	mesh.vertex_count = 4;
	mesh.index_count = 6;

	create_staged_buffer(mesh_vertices, sizeof(vertex) * mesh.vertex_count,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &mesh.vertex_buffer, &mesh.vertex_memory);
	create_staged_buffer(mesh_indices, sizeof(u32) * mesh.index_count,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT, &mesh.index_buffer, &mesh.index_memory);
	create_uniform_buffers();
	create_descriptor_sets();
}

void destroy_mesh()
{
	destroy_uniform_buffers();
	vkDestroyBuffer(device.handle, mesh.vertex_buffer, NULL);
	vkFreeMemory(device.handle, mesh.vertex_memory, NULL);
	vkDestroyBuffer(device.handle, mesh.index_buffer, NULL);
	vkFreeMemory(device.handle, mesh.index_memory, NULL);
}

void record_command_buffer(VkCommandBuffer cmdbuff, u32 img_idx, u32 frame_idx)
{
	VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	VkRenderPassBeginInfo render_pass_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
	VkRect2D scissor = { { 0, 0 }, surface.extent };
	VkViewport viewport = { 0.0f, 0.0f, (float)surface.extent.width, (float)surface.extent.height,
		0.0f, 1.0f };
	VkDeviceSize offsets[] = { 0 };
		
	render_pass_info.renderPass = g_default_render_pass;
	render_pass_info.framebuffer = surface.framebuffers[img_idx];
	render_pass_info.renderArea.offset.x = 0;
	render_pass_info.renderArea.offset.y = 0;
	render_pass_info.renderArea.extent = surface.extent;
	render_pass_info.clearValueCount = 1;
	render_pass_info.pClearValues = &clear_color;

	VKASSERT(vkBeginCommandBuffer(cmdbuff, &begin_info), "Failed to begin command buffer!");
	vkCmdBeginRenderPass(cmdbuff, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdSetViewport(cmdbuff, 0, 1, &viewport);
	vkCmdSetScissor(cmdbuff, 0, 1, &scissor);

	vkCmdBindPipeline(cmdbuff, VK_PIPELINE_BIND_POINT_GRAPHICS, g_default_pipeline);
	vkCmdBindDescriptorSets(cmdbuff, VK_PIPELINE_BIND_POINT_GRAPHICS, g_default_layout, 0, 1,
		&mesh.descriptor_sets[frame_idx], 0, NULL);

	vkCmdBindVertexBuffers(cmdbuff, 0, 1, &mesh.vertex_buffer, offsets);
	vkCmdBindIndexBuffer(cmdbuff, mesh.index_buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmdbuff, mesh.index_count, 1, 0, 0, 0);

	vkCmdEndRenderPass(cmdbuff);
	VKASSERT(vkEndCommandBuffer(cmdbuff), "Failed to end command buffer!");
}

void draw_frame()
{
	VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	VkPipelineStageFlags wait_stages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkPresentInfoKHR present_info = { 0 };
	u32 img_idx;
	VkResult error;

	vkWaitForFences(device.handle, 1, device.done_fences + frame_idx, VK_TRUE, U64_MAX);
	vkResetFences(device.handle, 1, device.done_fences + frame_idx);

	while (1)
	{
		error = vkAcquireNextImageKHR(device.handle, surface.swapchain, U64_MAX, device.image_semaphores[frame_idx],
			VK_NULL_HANDLE, &img_idx);

		if (error == VK_SUCCESS || error == VK_SUBOPTIMAL_KHR)
			break;
		else if (error == VK_ERROR_OUT_OF_DATE_KHR)
			recreate_swapchain();
		else
			VKASSERT(VK_ERROR_UNKNOWN, "Failed to acquire image!");
	}

	update_uniform_buffer(frame_idx);
	vkResetCommandBuffer(device.render_buffers[frame_idx], 0);
	record_command_buffer(device.render_buffers[frame_idx], img_idx, frame_idx);

	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = device.image_semaphores + frame_idx;
	submit_info.pWaitDstStageMask = wait_stages;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = device.render_buffers + frame_idx;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = device.done_semaphores + frame_idx;

	VKASSERT(vkQueueSubmit(device.graphics_queue, 1, &submit_info, device.done_fences[frame_idx]),
		"Failed to submit render commands!");

	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = device.done_semaphores + frame_idx;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &surface.swapchain;
	present_info.pImageIndices = &img_idx;
	present_info.pResults = NULL;

	error = vkQueuePresentKHR(surface.present_queue, &present_info);
	
	if (error == VK_ERROR_OUT_OF_DATE_KHR || error == VK_SUBOPTIMAL_KHR)
		recreate_swapchain();
	else if (error != VK_SUCCESS)
		VKASSERT(error, "Failed to present!");
	
	frame_idx = (frame_idx + 1) % g_rndcfg.parallel_frames;
}

void initialize_renderer()
{
	instance = create_instance();
	create_device(&device);
	create_descriptor_pool();
	create_descriptor_set_layout();
	create_mesh();
}

void destroy_renderer()
{
	destroy_mesh();
	vkDestroyDescriptorSetLayout(device.handle, g_default_descriptor_layout, NULL);
	destroy_descriptor_pool();
	destroy_command_pools(&device);
	vkDestroyDevice(device.handle, NULL);
	vkDestroyInstance(instance, NULL);
}