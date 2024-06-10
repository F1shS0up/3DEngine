#pragma once
#include "camera.h"
#include "ecs/coordinator.hpp"
#include "fps_camera.h"
#include "glfw/glfw3.h"
#include "key_holder.h"
#include "mesh_renderer.h"
#include "options.h"

struct window_info
{
	int monitorHeight, monitorWidth;
	int windowHeight, windowWidth;
	double monitorAspectRatio, windowAspectRatio;
};
struct engine
{
private:
	static engine* instance;

public:
	static engine* Instance()
	{
		return instance;
	}
	engine(float& deltaTime) : deltaTime(deltaTime)
	{
		instance = this;
	};

	void Init();

	void ECSInit();

	void RegisterSystems();

	void Update();

	void Render();

	void SetShaderVariables();

	void Terminate();

	void UpdateResolution(int width, int height);

private:
	void MousePositionUpdate();
	void Clean();
	void InitGL();
	void GetAssetPath();
	void CreateFBO();

public:
	float& deltaTime;
	bool running;
	GLFWwindow* window;
	std::string assetPath;
	options opt {options::AutoBlackBars, options::Windowed, 3840, 2160, 0, true, false, false, "3DEngine"};
	camera_system cameraSystem;
	key_holder keyHolder;

	double mouseX, mouseY;
	double windowMouseX, windowMouseY;

	window_info windowInfo;

private:
	static const unsigned int TEXTURE_COUNT = 2;
	unsigned int FBO, DEPTH, TEX[TEXTURE_COUNT];

	unsigned int VAO, VBO;
};