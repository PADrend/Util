/*
	This file is part of the Util library.
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_VULKAN

#include "VulkanWindowTest.h"
#include "../Timer.h"
#include "../Utils.h"
#include "../UI/UI.h"

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(VulkanWindowTest);

using namespace Util;
using namespace Util::UI;

const std::string vertexShader = R"vs(
	#version 450
	#extension GL_ARB_separate_shader_objects : enable

	out gl_PerVertex {
			vec4 gl_Position;
	};

	layout(location = 0) out vec3 fragColor;

	vec2 positions[3] = vec2[](
		vec2(0.0, -0.5),
		vec2(0.5, 0.5),
		vec2(-0.5, 0.5)
	);

	vec3 colors[3] = vec3[](
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);

	void main() {
		gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
		fragColor = colors[gl_VertexIndex];
	}
)vs";

const std::string fragmentShader = R"fs(
	#version 450
	#extension GL_ARB_separate_shader_objects : enable

	layout(location = 0) in vec3 fragColor;

	layout(location = 0) out vec4 outColor;

	void main() {
		outColor = vec4(fragColor, 1.0);
	}
)fs";

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

vk::UniqueShaderModule createShaderModule(vk::Device& device, const std::string& code, const std::string& name, shaderc_shader_kind kind) {
	static shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);
	shaderc::SpvCompilationResult shaderModule = compiler.CompileGlslToSpv(code, kind, name.c_str(), options);
	if (shaderModule.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << shaderModule.GetErrorMessage();
	}
	std::vector<uint32_t> shaderCode = std::vector<uint32_t>{ shaderModule.cbegin(), shaderModule.cend() };
	return device.createShaderModuleUnique({ {}, shaderCode.size() * sizeof(uint32_t), shaderCode.data() });
}

void VulkanWindowTest::test() {
	std::cout << std::endl;

	// create window
	Util::UI::Window::Properties properties;
	properties.positioned = false;
	properties.clientAreaWidth = 640;
	properties.clientAreaHeight = 480;
	properties.title = "Window Test";
	properties.compatibilityProfile = false;
	std::unique_ptr<Window> window;
	CPPUNIT_ASSERT_NO_THROW(window = Util::UI::createWindow(properties));
	
	// Create Instance
	vk::ApplicationInfo appInfo(
		properties.title.c_str(), 1,
		nullptr, 0,
		VK_API_VERSION_1_1
	);
	
	std::vector<const char*> layerNames = {"VK_LAYER_LUNARG_standard_validation"};
	std::vector<const char*> requiredExtensions = window->getAPIExtensions();
	requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	auto instance = vk::createInstanceUnique({{},
		&appInfo,
		static_cast<uint32_t>(layerNames.size()),
		layerNames.data(),
		static_cast<uint32_t>(requiredExtensions.size()),
		requiredExtensions.data()
	});
	
	// setup debug callback
	vk::DispatchLoaderDynamic dldy;
	dldy.init(*instance);
	
	auto debugMessenger = instance->createDebugUtilsMessengerEXTUnique({ {},
		//vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		debugCallback
	}, nullptr, dldy);
	
	// create surface
	vk::UniqueSurfaceKHR surface(window->createSurface(*instance), *instance);
	
	// create physical device
	auto physicalDevices = instance->enumeratePhysicalDevices();
	CPPUNIT_ASSERT(physicalDevices.size() > 0);
	auto physicalDevice = physicalDevices.front(); // use first available device
	
	// get queue families
	auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	std::vector<uint32_t> familyIndices = { static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(),
		std::find_if(queueFamilyProperties.begin(), queueFamilyProperties.end(), [](const vk::QueueFamilyProperties& qfp) {
			return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
		})
	))};
	
	CPPUNIT_ASSERT(queueFamilyProperties[familyIndices.front()].queueFlags & vk::QueueFlagBits::eGraphics);	
	if(!physicalDevice.getSurfaceSupportKHR(familyIndices.front(), surface.get())) {
		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
			if (physicalDevice.getSurfaceSupportKHR(i, surface.get())) {
				familyIndices.emplace_back(i);
				break;
			}
		}
	}

	float queuePriority = 0.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	for (uint32_t index : familyIndices)
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), index, 1, &queuePriority);
	
	// create logical device
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	auto device = physicalDevice.createDeviceUnique({ {}, 
		static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), 
		0, nullptr, 
		static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data()
	}, nullptr, dldy);
	
	dldy.init(*instance, *device);
	auto graphicsQueueIndex = familyIndices.front();
	//auto presentQueueIndex = familyIndices.back();
	auto graphicsQueue = device->getQueue(familyIndices.front(), 0);
	auto presentQueue = device->getQueue(familyIndices.back(), 0);
		
	// create swap chain
	auto capabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	auto formats = physicalDevice.getSurfaceFormatsKHR(*surface);
	auto presentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
	vk::SurfaceFormatKHR surfaceFormat = {
		vk::Format::eB8G8R8A8Unorm,
		vk::ColorSpaceKHR::eSrgbNonlinear
	};
	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
	CPPUNIT_ASSERT(!formats.empty() && !presentModes.empty());
	CPPUNIT_ASSERT(std::find(formats.begin(), formats.end(), surfaceFormat) != formats.end());
	CPPUNIT_ASSERT(std::find(presentModes.begin(), presentModes.end(), presentMode) != presentModes.end());
	
	vk::Extent2D swapChainExtent = { 
		std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, properties.clientAreaWidth)),
		std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, properties.clientAreaHeight)),
	};	
	uint32_t imageCount = capabilities.minImageCount + 1;
	if(capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
		imageCount = capabilities.maxImageCount;
	
	vk::SwapchainCreateInfoKHR swapChainCreateInfo { {},
		*surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace,
		swapChainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment,
		vk::SharingMode::eExclusive, 0, nullptr,
		capabilities.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
		presentMode, true
	};
	if(familyIndices.size() > 1) {
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapChainCreateInfo.queueFamilyIndexCount = familyIndices.size();
		swapChainCreateInfo.pQueueFamilyIndices = familyIndices.data();
	}
	auto swapChain = device->createSwapchainKHRUnique(swapChainCreateInfo);
	std::vector<vk::Image> swapChainImages = device->getSwapchainImagesKHR(swapChain.get());

	std::vector<vk::UniqueImageView> imageViews;
	for(auto& image : swapChainImages) {
		imageViews.emplace_back(device->createImageViewUnique({ {}, 
			image, vk::ImageViewType::e2D, surfaceFormat.format,
			{ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA },
			{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
		}));
	}
	
	// --------------------------------------------
	// create graphics pipeline
	
	// compile shaders
	auto vertexShaderModule = createShaderModule(*device, vertexShader, "vertex shader", shaderc_glsl_vertex_shader);
	auto fragmentShaderModule = createShaderModule(*device, fragmentShader, "fragment shader", shaderc_glsl_fragment_shader);	
	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages = { 
		{ {}, vk::ShaderStageFlagBits::eVertex, *vertexShaderModule, "main" },
		{ {}, vk::ShaderStageFlagBits::eFragment, *fragmentShaderModule, "main" }
	};

	// vertex input
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly { {}, vk::PrimitiveTopology::eTriangleList, false };

	// rasterizer
	vk::Viewport viewport { 0.0f, 0.0f, static_cast<float>(properties.clientAreaWidth), static_cast<float>(properties.clientAreaHeight), 0.0f, 1.0f };
	vk::Rect2D scissor {{0, 0}, swapChainExtent};
	vk::PipelineViewportStateCreateInfo viewportState { {}, 1, &viewport, 1, &scissor };

	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.frontFace = vk::FrontFace::eClockwise;
	rasterizer.lineWidth = 1.0f;
	
	vk::PipelineMultisampleStateCreateInfo multisampling;

	// color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment { 
		false,  
		vk::BlendFactor::eZero, vk::BlendFactor::eOne, vk::BlendOp::eAdd,
		vk::BlendFactor::eZero, vk::BlendFactor::eOne, vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	};

	vk::PipelineColorBlendStateCreateInfo colorBlending { {}, false, vk::LogicOp::eCopy, 1, &colorBlendAttachment };

	// render pass
	vk::AttachmentDescription colorAttachment { {},
		surfaceFormat.format, 
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, 
		vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, 
		vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
	};
	vk::AttachmentReference colourAttachmentRef { 0, vk::ImageLayout::eColorAttachmentOptimal };
	vk::SubpassDescription subpass { {}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colourAttachmentRef };

	vk::SubpassDependency subpassDependency {
		VK_SUBPASS_EXTERNAL, 0,
		vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
		{}, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
	};

	auto renderPass = device->createRenderPassUnique({ {}, 
		1, &colorAttachment, 
		1, &subpass,
		1, &subpassDependency
	});

	// create pipeline
	auto pipelineLayout = device->createPipelineLayoutUnique({});
	auto pipeline = device->createGraphicsPipelineUnique({}, { {}, 
		static_cast<uint32_t>(pipelineShaderStages.size()), pipelineShaderStages.data(),
		&vertexInputInfo, &inputAssembly, nullptr, &viewportState, &rasterizer, &multisampling,
		nullptr, &colorBlending, nullptr, *pipelineLayout, *renderPass, 0
	});

	// create framebuffers
	std::vector<vk::UniqueFramebuffer> framebuffers;
	for(auto& view : imageViews) {
		framebuffers.emplace_back(device->createFramebufferUnique({ {},
			*renderPass, 1, &(*view),
			swapChainExtent.width, swapChainExtent.height, 1
		}));
	}
		
	// --------------------------------------------
	// command queue
	
	// create command buffers
	auto commandPool = device->createCommandPoolUnique({ {}, graphicsQueueIndex });

	std::vector<vk::UniqueCommandBuffer> commandBuffers = device->allocateCommandBuffersUnique({
		*commandPool, 
		vk::CommandBufferLevel::ePrimary, 
		static_cast<uint32_t>(framebuffers.size())
	});
	
	// record commands
	for(size_t i = 0; i < commandBuffers.size(); ++i) {
		commandBuffers[i]->begin({vk::CommandBufferUsageFlagBits::eSimultaneousUse});
		vk::ClearValue clearColor(vk::ClearColorValue{std::array<float,4>{0.0f, 0.0f, 0.0f, 1.0f}});
		commandBuffers[i]->beginRenderPass({
			*renderPass, *framebuffers[i],
			vk::Rect2D{ {0, 0}, swapChainExtent },
			1, &clearColor
		}, vk::SubpassContents::eInline);
		commandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
		commandBuffers[i]->draw(3, 1, 0, 0);
		
		commandBuffers[i]->endRenderPass();
		commandBuffers[i]->end();
	}
	
	
	// --------------------------------------------
	// draw
	
	auto imageAvailableSemaphore = device->createSemaphoreUnique({});
	auto renderFinishedSemaphore = device->createSemaphoreUnique({});
	
	for(uint_fast32_t round = 0; round < 100; ++round) {
		auto imageIndex = device->acquireNextImageKHR(*swapChain, std::numeric_limits<uint64_t>::max(), *imageAvailableSemaphore, {});

		vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		graphicsQueue.submit({{
			1, &imageAvailableSemaphore.get(), &waitStageMask, 
			1, &commandBuffers[imageIndex.value].get(), 
			1, &renderFinishedSemaphore.get() 
		}}, {});

		presentQueue.presentKHR({
			1, &renderFinishedSemaphore.get(),
			1, &swapChain.get(),
			&imageIndex.value
		});
		
	}
	device->waitIdle();
}

#endif /* UTIL_HAVE_LIB_VULKAN */