#include "engine.h"

#include "GLFW/glfw3.h"
#include "camera_movement.h"
#include "fps_camera.h"
#include "glad/glad.h"
#include "light_manager.h"
#include "mesh.h"
#include "mesh_renderer.h"
#include "resource_manager.h"
#include "stb_image.h"
#include "transform.h"

#include <algorithm>
#include <freetype/freetype.h>
#include <ft2build.h>
#include <iostream>
#include <math.h>
#include <windows.h>

Coordinator gCoordinator;
std::shared_ptr<mesh_renderer_system> meshRendererSystem;
std::shared_ptr<fps_camera_system> fpsCameraSystem;
std::shared_ptr<camera_movement_system> cameraMovementSystem;
std::shared_ptr<point_light_manager> pointLightManager;
std::shared_ptr<directional_light_manager> directionalLightManager;
engine* engine::instance = nullptr;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		engine::Instance()->keyHolder.keysDown[key] = false;
	}
	else if (action == GLFW_PRESS)
	{
		engine::Instance()->keyHolder.keysDown[key] = true;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		engine::Instance()->running = false;
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_RELEASE)
	{
		engine::Instance()->keyHolder.mouseButtonsDown[button] = false;
	}
	else if (action == GLFW_PRESS)
	{
		engine::Instance()->keyHolder.mouseButtonsDown[button] = true;
	}
}
void engine::Init()
{
	cameraSystem.CreateCamera(camera(3840, 2160, 1.f, 100.0f, 60.0f, glm::vec3(0, 0, 0), 0, -90));
	InitGL();

	// m = new model(new mesh((assetPath + "Models/backpack.obj").c_str()), transform(glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
	resource_manager::LoadTexture((assetPath + "Engine/Textures/white.png").c_str(), "DEFAULT", false);
	resource_manager::LoadTexture((assetPath + "Models/Backpack/diffuse.png").c_str(), "diffuse", false);
	resource_manager::LoadTexture((assetPath + "Models/Backpack/specular.jpg").c_str(), "specular", false);
	resource_manager::LoadShader((assetPath + "Shaders/3Dtest.vert").c_str(), (assetPath + "Shaders/3Dtest.frag").c_str(), nullptr, "3Dtest");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default.frag").c_str(), nullptr, "default");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default_unlit.frag").c_str(), nullptr, "default_unlit");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default_lit.frag").c_str(), nullptr, "default_lit");
	resource_manager::LoadShader((assetPath + "Shaders/directional_light_depth.vert").c_str(), (assetPath + "Shaders/directional_light_depth.frag").c_str(), nullptr, "DIRECTIONAL_SHADOW_MAPPING");
	resource_manager::LoadShader((assetPath + "Shaders/point_light_depth.vert").c_str(), (assetPath + "Shaders/point_light_depth.frag").c_str(), (assetPath + "Shaders/point_light_depth.geom").c_str(),
								 "POINT_SHADOW_MAPPING");
	resource_manager::LoadMesh((assetPath + "Models/backpack.obj").c_str(), "backpack");
	resource_manager::LoadMesh((assetPath + "Models/untitled.obj").c_str(), "untitled");
	resource_manager::LoadMesh((assetPath + "Models/box.obj").c_str(), "box");
	resource_manager::LoadMesh((assetPath + "Engine/sphere.obj").c_str(), "SPHERE");
	resource_manager::LoadMesh((assetPath + "Engine/plane.obj").c_str(), "PLANE");
	resource_manager::LoadMesh((assetPath + "Models/selfShadowTest.obj").c_str(), "selfShadowTest");

	ECSInit();
}

void engine::ECSInit()
{
	gCoordinator.Init();
	gCoordinator.RegisterComponent<transform>();
	gCoordinator.RegisterComponent<mesh_renderer>();
	gCoordinator.RegisterComponent<fps_camera>();
	gCoordinator.RegisterComponent<camera_movement>();
	gCoordinator.RegisterComponent<point_light>();
	gCoordinator.RegisterComponent<directional_light>();

	RegisterSystems();
	material_lit* mat = new material_lit();
	mat->diffuseMap = resource_manager::GetTexture("DEFAULT");
	mat->specularMap = resource_manager::GetTexture("DEFAULT");
	mat->shininess = 1.0f;
	material_lit* matBackpack = new material_lit();
	matBackpack->diffuseMap = resource_manager::GetTexture("diffuse");
	matBackpack->specularMap = resource_manager::GetTexture("specular");
	matBackpack->shininess = 1.0f;

	material_lit* noShine = new material_lit();
	noShine->diffuseMap = resource_manager::GetTexture("DEFAULT");
	noShine->specularMap = resource_manager::GetTexture("DEFAULT");
	noShine->shininess = 0.0f;
	noShine->specular = glm::vec3(0.2f);

	Entity e = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(e, transform {glm::vec3(0, 1, 2), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	gCoordinator.AddComponent(e, mesh_renderer {resource_manager::GetMesh("box"), mat, resource_manager::GetShader("default_lit")});
	Entity e2 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(e2, transform {glm::vec3(0, -2, 0), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)});
	gCoordinator.AddComponent(e2, mesh_renderer {resource_manager::GetMesh("PLANE"), noShine, resource_manager::GetShader("default_lit")});
	Entity e3 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(e3, transform {glm::vec3(-2, 8, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	gCoordinator.AddComponent(e3, mesh_renderer {resource_manager::GetMesh("backpack"), matBackpack, resource_manager::GetShader("default_lit")});
	Entity e4 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(e4, transform {glm::vec3(4, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	gCoordinator.AddComponent(e4, mesh_renderer {resource_manager::GetMesh("backpack"), matBackpack, resource_manager::GetShader("default_lit")});
	// Entity e4 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e4, transform {glm::vec3(-10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e4, mesh_renderer {resource_manager::GetMesh("backpack"), mat, resource_manager::GetShader("default_lit")});
	// Entity e5 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e5, transform {glm::vec3(5, 0, 5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e5, mesh_renderer {resource_manager::GetMesh("backpack"), mat, resource_manager::GetShader("default_lit")});
	// Entity e6 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e6, transform {glm::vec3(5, 0, -5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e6, mesh_renderer {resource_manager::GetMesh("backpack"), mat, resource_manager::GetShader("default_lit")});
	// Entity e7 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e7, transform {glm::vec3(-5, 0, -5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e7, mesh_renderer {resource_manager::GetMesh("backpack"), mat, resource_manager::GetShader("default_lit")});
	// Entity e8 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e8, transform {glm::vec3(-5, 0, 5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e8, mesh_renderer {resource_manager::GetMesh("backpack"), mat, resource_manager::GetShader("default_lit")});

	Entity fpsCamera = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(fpsCamera, fps_camera {cameraSystem.GetCurrentCamera()});
	gCoordinator.AddComponent(fpsCamera, camera_movement {cameraSystem.GetCurrentCamera()});

	material_unlit* greenMat = new material_unlit();
	greenMat->map = resource_manager::GetTexture("DEFAULT");
	greenMat->ambient = glm::vec3(0.0f, 1.0f, 0.0f);

	material_unlit* blueMat = new material_unlit();
	blueMat->map = resource_manager::GetTexture("DEFAULT");
	blueMat->ambient = glm::vec3(0.0f, 0.0f, 1.0f);

	Entity directionalLight = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(directionalLight,
							  directional_light(glm::vec3(1, -1, -1), 1.f, 100.f, 25.f, 1.f, true, glm::vec3(0.65f, 0.6f, 0.3f), glm::vec3(0.72f, 0.7f, 0.6f), glm::vec3(1, 0.925f, 0.753f)));

	//
	Entity pointLight2 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(pointLight2, transform {glm::vec3(1, 1, 0), glm::vec3(0, 0, 0), glm::vec3(.2f)});
	gCoordinator.AddComponent(pointLight2, point_light {1.0f, 0.09f, 0.032f, 1.f, true, glm::vec3(0, 0, 0.1f), glm::vec3(0, 0, 0.5f), glm::vec3(0, 0, 1)});
	gCoordinator.AddComponent(pointLight2, mesh_renderer {resource_manager::GetMesh("SPHERE"), blueMat, resource_manager::GetShader("default_unlit")});

	pointLightManager->Init();
	meshRendererSystem->Init();
	directionalLightManager->Init();
}

void engine::RegisterSystems()
{
	Signature sig;
	sig.set(gCoordinator.GetComponentType<transform>());
	sig.set(gCoordinator.GetComponentType<mesh_renderer>());
	gCoordinator.SetSystemSignature<mesh_renderer_system>(sig);
	meshRendererSystem = gCoordinator.RegisterSystem<mesh_renderer_system>();

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<fps_camera>());
	gCoordinator.SetSystemSignature<fps_camera_system>(sig);
	fpsCameraSystem = gCoordinator.RegisterSystem<fps_camera_system>();

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<camera_movement>());
	gCoordinator.SetSystemSignature<camera_movement_system>(sig);
	cameraMovementSystem = gCoordinator.RegisterSystem<camera_movement_system>();

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<point_light>());
	sig.set(gCoordinator.GetComponentType<transform>());
	gCoordinator.SetSystemSignature<point_light_manager>(sig);
	pointLightManager = gCoordinator.RegisterSystem<point_light_manager>();

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<directional_light>());
	gCoordinator.SetSystemSignature<directional_light_manager>(sig);
	directionalLightManager = gCoordinator.RegisterSystem<directional_light_manager>();
}

void engine::Update()
{
	MousePositionUpdate();
	std::cout << "FPS: " << 1.f / deltaTime << std::endl;
	fpsCameraSystem->Update(deltaTime);
	cameraMovementSystem->Update(deltaTime);
}

void engine::Render()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	directionalLightManager->SetLightSpaceMatrix();

	directionalLightManager->RenderFromLightsPOV();

	pointLightManager->RenderFromLightsPOV();

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputHeight);

	directionalLightManager->SetShaderVariables();
	pointLightManager->SetShaderVariables();
	SetShaderVariables();
	meshRendererSystem->Render();
	// m->t.rotation = glm::vec3(0, glm::radians((float)counter), 0);
	// m->Render(resource_manager::GetShader("default"));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBlitFramebuffer(0, 0, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputHeight, 0, 0, windowInfo.windowWidth, windowInfo.windowHeight, GL_COLOR_BUFFER_BIT,
					  GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void engine::SetShaderVariables()
{
	resource_manager::GetShader("default_lit")->SetMatrix4("view", cameraSystem.GetCurrentCamera()->GetViewMatrix(), true);
	resource_manager::GetShader("default_lit")->SetMatrix4("projection", cameraSystem.GetCurrentCamera()->projection);
	resource_manager::GetShader("default_lit")->SetVector3f("viewPos", cameraSystem.GetCurrentCamera()->GetPosition());

	resource_manager::GetShader("default_unlit")->SetMatrix4("view", cameraSystem.GetCurrentCamera()->GetViewMatrix(), true);
	resource_manager::GetShader("default_unlit")->SetMatrix4("projection", cameraSystem.GetCurrentCamera()->projection);
}

void engine::Terminate()
{
	Clean();
}

void engine::Clean() { }
void engine::MousePositionUpdate()
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	camera* cam = cameraSystem.GetCurrentCamera();

	windowMouseX = x;
	windowMouseY = y;
	x *= (float)cam->outputWidth / (float)windowInfo.windowWidth;
	y *= (float)cam->outputHeight / (float)windowInfo.windowHeight;
	mouseX = x;
	mouseY = y;
}
void engine::InitGL()
{
	GetAssetPath();

	/* Initialize the library */
	std::cout << "Initializing glfw..." << std::endl;
	if (!glfwInit())
	{
		std::cout << "Error while initializing glfw!" << std::endl;
		running = false;
		return;
	}

	glfwWindowHint(GLFW_RESIZABLE, opt.resizable);
	/* Create a windowed mode window and its OpenGL context */
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	windowInfo.monitorWidth = mode->width;
	windowInfo.monitorHeight = mode->height;

	if (opt.windowOption == options::Fullscreen)
	{
		window = glfwCreateWindow(mode->width, mode->height, opt.windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
		windowInfo.windowWidth = mode->width;
		windowInfo.windowHeight = mode->height;
	}
	else if (opt.windowOption == options::Borderless)
	{
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		window = glfwCreateWindow(mode->width, mode->height, opt.windowName.c_str(), nullptr, nullptr);
		windowInfo.windowWidth = mode->width;
		windowInfo.windowHeight = mode->height;
	}
	else
	{
		window = glfwCreateWindow(opt.wndWidth, opt.wndHeight, opt.windowName.c_str(), nullptr, nullptr);
		windowInfo.windowWidth = opt.wndWidth;
		windowInfo.windowHeight = opt.wndHeight;
	}

	if (!window)
	{
		glfwTerminate();
		running = false;
		return;
	}

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	// glfwSetScrollCallback(window, ScrollCallback);
	// glfwSetWindowSizeCallback(window, WindowResizeCallback);
	// glfwSetCharCallback(window, CharacterCallback);
	// glfwSetWindowFocusCallback(window, WindowFocusCallback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	std::cout << "Initializing GLAD..." << std::endl;
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Error while initialize GLAD" << std::endl;
		running = false;
		return;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(opt.vSync);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_CULL_FACE);

	CreateFBO();
}

void engine::GetAssetPath()
{

#ifdef _WIN32
	char pBuf[256];
	size_t len = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, len);
	assetPath = pBuf;
	while (pBuf[assetPath.length() - 1] != '\\')
	{
		assetPath.pop_back();
	}
	assetPath += "Assets\\";
#endif

#ifdef __linux__
	// TODO
#endif
}

void engine::CreateFBO()
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glGenTextures(TEXTURE_COUNT, TEX);

	for (int i = 0; i < TEXTURE_COUNT; i++)
	{
		glBindTexture(GL_TEXTURE_2D, TEX[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputWidth, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, TEX[i], 0);
	}

	glGenRenderbuffers(1, &DEPTH);
	glBindRenderbuffer(GL_RENDERBUFFER, DEPTH);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputWidth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DEPTH);

	unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void engine::UpdateResolution(int width, int height)
{
	for (int i = 0; i < TEXTURE_COUNT; i++)
	{
		glBindTexture(GL_TEXTURE_2D, TEX[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, width, height);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
