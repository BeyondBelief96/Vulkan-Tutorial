#include "application.h"
#include "simple_render_system.h"
#include "keyboard_movement_controller.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORECE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <chrono>

namespace Trek
{
	Application::Application()
	{
		loadGameObjects();
	}

	void Application::run()
	{
		const SimpleRenderSystem simpleRenderSystem{ trekDevice, trekRenderer.getSwapChainRenderPass() };
		TrekCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = TrekGameObject::createGameObject();
		const KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while(!trekWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			const float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(trekWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform2d.translation, viewerObject.transform2d.rotation);
			const float aspect = trekRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.f);
			if(const auto commandBuffer = trekRenderer.beginFrame())
			{
				trekRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				trekRenderer.endSwapChainRenderPass(commandBuffer);
				trekRenderer.endFrame();
			}
		}

		vkDeviceWaitIdle(trekDevice.device());
	}

	void Application::loadGameObjects()
	{
		const std::shared_ptr<TrekModel> model = TrekModel::createModelFromFile(trekDevice, "models/smooth_vase.obj");
		auto cube = TrekGameObject::createGameObject();
		cube.model = model;
		cube.transform2d.translation = { .0f, .0f, 2.5f };
		cube.transform2d.scale = glm::vec3{ 3.f };

		gameObjects.push_back(std::move(cube));
	}
}

