#include "engine.h"

#include "GLFW/glfw3.h"
#include "camera_movement.h"
#include "fps_camera.h"
#include "glad/glad.h"
#include "light_manager.h"
#include "material.h"
#include "mesh_renderer.h"
#include "resource_manager.h"
#include "skybox.h"
#include "stb_image.h"
#include "transform.h"

#include "json/json.h"
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
std::shared_ptr<skybox_system> skyboxSystem;
std::shared_ptr<transform_system> transformSystem;
std::shared_ptr<light_manager> lightManager;
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
	InitGL();
	running = true;
	cameraSystem.CreateCamera(camera(0, 0, .1f, 1000.0f, 45.0f, glm::vec4(0, 0, 0, 1), 0, 90));
	CreateFBO();

	float startTime = glfwGetTime();
	// m = new model(new mesh((assetPath + "Models/backpack.obj").c_str()), transform(glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
	resource_manager::LoadTexture((assetPath + "Engine/Textures/white.png").c_str(), "DEFAULT");
	resource_manager::LoadTexture((assetPath + "Engine/Textures/defaultNormalMap.png").c_str(), "DEFAULT_NORMAL_MAP");
	resource_manager::LoadTexture((assetPath + "Models/Backpack/diffuse.png").c_str(), "diffuse");
	resource_manager::LoadTexture((assetPath + "Models/Backpack/specular.jpg").c_str(), "specular");
	resource_manager::LoadTexture((assetPath + "Textures/rustediron2_roughness.png").c_str(), "rustedRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/rustediron2_normal.png").c_str(), "rustedNormal");
	resource_manager::LoadTexture((assetPath + "Textures/rustediron2_metallic.png").c_str(), "rustedMetallic");
	resource_manager::LoadTexture((assetPath + "Textures/rustediron2_basecolor.png").c_str(), "rustedAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SandDesertAlbedo.png").c_str(), "SandAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SandDesertAO.png").c_str(), "SandAO");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SandDesertNormal.png").c_str(), "SandNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SandDesertRoughness.png").c_str(), "SandRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SpaceBlanketAlbedo.png").c_str(), "SpaceBlanketAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SpaceBlanketAO.png").c_str(), "SpaceBlanketAO");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SpaceBlanketNormal.png").c_str(), "SpaceBlanketNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SpaceBlanketRoughness.png").c_str(), "SpaceBlanketRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/SpaceBlanketMetallic.png").c_str(), "SpaceBlanketMetallic");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/WoodAlbedo.png").c_str(), "WoodAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/WoodAO.png").c_str(), "WoodAO");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/WoodNormal.png").c_str(), "WoodNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/WoodRoughness.png").c_str(), "WoodRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/FabricAlbedo.png").c_str(), "FabricAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/FabricAO.png").c_str(), "FabricAO");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/FabricNormal.png").c_str(), "FabricNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/FabricRoughness.png").c_str(), "FabricRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/ScifiPanelAlbedo.png").c_str(), "ScifiPanelAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/ScifiPanelAO.png").c_str(), "ScifiPanelAO");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/ScifiPanelNormal.png").c_str(), "ScifiPanelNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/ScifiPanelRoughness.png").c_str(), "ScifiPanelRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/ScifiPanelMetallic.png").c_str(), "ScifiPanelMetallic");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/AcousticFoam_albedo.png").c_str(), "AcousticFoamAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/AcousticFoam_ao.png").c_str(), "AcousticFoamAO");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/AcousticFoam_normal.png").c_str(), "AcousticFoamNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/AcousticFoam_roughness.png").c_str(), "AcousticFoamRoughness");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/AcousticFoam_metallic.png").c_str(), "AcousticFoamMetallic");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/AcousticFoam_height.png").c_str(), "AcousticFoamHeight");

	resource_manager::LoadTexture((assetPath + "Textures/surfaces/brick_diffuse.jpg").c_str(), "BrickAlbedo");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/brick_normal.jpg").c_str(), "BrickNormal");
	resource_manager::LoadTexture((assetPath + "Textures/surfaces/brick_height.jpg").c_str(), "BrickHeight");
	float endTime = glfwGetTime();
	std::cout << "Loaded textures in: " << endTime - startTime << "s" << std::endl;

	startTime = glfwGetTime();
	resource_manager::LoadShader((assetPath + "Shaders/3Dtest.vert").c_str(), (assetPath + "Shaders/3Dtest.frag").c_str(), nullptr, nullptr, nullptr, "3Dtest");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default.frag").c_str(), nullptr, nullptr, nullptr, "default");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default_unlit.frag").c_str(), nullptr, nullptr, nullptr, "default_unlit");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default_lit.frag").c_str(), nullptr, nullptr, nullptr, "default_lit");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default_lit_no_textures.frag").c_str(), nullptr, nullptr, nullptr, "default_lit_no_textures");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/default_pbr.frag").c_str(), nullptr, nullptr, nullptr, "default_pbr");
	resource_manager::LoadShader((assetPath + "Shaders/default.vert").c_str(), (assetPath + "Shaders/transparent_pbr.frag").c_str(), nullptr, nullptr, nullptr, "transparent_pbr");
	resource_manager::LoadShader((assetPath + "Shaders/skybox.vert").c_str(), (assetPath + "Shaders/skybox.frag").c_str(), nullptr, nullptr, nullptr, "skybox");
	resource_manager::LoadShader((assetPath + "Shaders/directional_light_depth.vert").c_str(), (assetPath + "Shaders/directional_light_depth.frag").c_str(), nullptr, nullptr, nullptr,
								 "DIRECTIONAL_SHADOW_MAPPING");
	resource_manager::LoadShader((assetPath + "Shaders/point_light_depth.vert").c_str(), (assetPath + "Shaders/point_light_depth.frag").c_str(), (assetPath + "Shaders/point_light_depth.geom").c_str(),
								 nullptr, nullptr, "POINT_SHADOW_MAPPING");
	endTime = glfwGetTime();
	std::cout << "Loaded shaders in: " << endTime - startTime << "s" << std::endl;

	// resource_manager::LoadModel((assetPath + "Models/backpack.obj").c_str(), "backpack");
	// resource_manager::LoadModel((assetPath + "Models/box.obj").c_str(), "box");
	// resource_manager::LoadModel((assetPath + "Engine/sphere.obj").c_str(), "SPHERE");
	// resource_manager::LoadModel((assetPath + "Engine/plane.obj").c_str(), "PLANE");
	startTime = glfwGetTime();
	// resource_manager::LoadModelFromBinary((assetPath + "Models/formula.bin").c_str(), "Nissan");
	resource_manager::LoadModelFromBinary((assetPath + "Models/formula.bin").c_str(), "Nissan2");
	//  resource_manager::LoadModel((assetPath + "Models/NissanMat.obj").c_str(), "Nissan2");

	endTime = glfwGetTime();
	std::cout << "Loaded models in: " << endTime - startTime << "s" << std::endl;

	resource_manager::LoadCubemap((assetPath + "Textures/skybox").c_str(), ".png", "skybox");

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
	gCoordinator.RegisterComponent<skybox>();
	gCoordinator.RegisterComponent<light>();

	RegisterSystems();
	material_pbr* sand = new material_pbr();
	sand->albedoMap = resource_manager::GetTexture("SandAlbedo");
	sand->roughnessMap = resource_manager::GetTexture("SandRoughness");
	sand->normalMap = resource_manager::GetTexture("SandNormal");
	sand->aoMap = resource_manager::GetTexture("SandAO");

	material_pbr* spaceBlanket = new material_pbr();
	spaceBlanket->albedoMap = resource_manager::GetTexture("SpaceBlanketAlbedo");
	spaceBlanket->roughnessMap = resource_manager::GetTexture("SpaceBlanketRoughness");
	spaceBlanket->normalMap = resource_manager::GetTexture("SpaceBlanketNormal");
	spaceBlanket->aoMap = resource_manager::GetTexture("SpaceBlanketAO");
	spaceBlanket->metallicMap = resource_manager::GetTexture("SpaceBlanketMetallic");

	transparent_pbr* rusted = new transparent_pbr();
	rusted->albedoMap = resource_manager::GetTexture("rustedAlbedo");
	rusted->metallicMap = resource_manager::GetTexture("rustedMetallic");
	rusted->roughnessMap = resource_manager::GetTexture("rustedRoughness");
	rusted->normalMap = resource_manager::GetTexture("rustedNormal");

	transparent_pbr* wood = new transparent_pbr();
	wood->albedoMap = resource_manager::GetTexture("WoodAlbedo");
	wood->roughnessMap = resource_manager::GetTexture("WoodRoughness");
	wood->normalMap = resource_manager::GetTexture("WoodNormal");
	wood->aoMap = resource_manager::GetTexture("WoodAO");

	transparent_pbr* fabric = new transparent_pbr();
	fabric->albedoMap = resource_manager::GetTexture("FabricAlbedo");
	fabric->roughnessMap = resource_manager::GetTexture("FabricRoughness");
	fabric->normalMap = resource_manager::GetTexture("FabricNormal");
	fabric->aoMap = resource_manager::GetTexture("FabricAO");

	material_pbr* acousticFoam = new material_pbr();
	acousticFoam->albedoMap = resource_manager::GetTexture("AcousticFoamAlbedo");
	acousticFoam->roughnessMap = resource_manager::GetTexture("AcousticFoamRoughness");
	acousticFoam->normalMap = resource_manager::GetTexture("AcousticFoamNormal");
	acousticFoam->aoMap = resource_manager::GetTexture("AcousticFoamAO");
	acousticFoam->metallicMap = resource_manager::GetTexture("AcousticFoamMetallic");
	acousticFoam->heightMap = resource_manager::GetTexture("AcousticFoamHeight");
	acousticFoam->uvMultiplier = 10;

	material_pbr* scifiPanelPlane = new material_pbr();
	scifiPanelPlane->albedoMap = resource_manager::GetTexture("BrickAlbedo");
	scifiPanelPlane->normalMap = resource_manager::GetTexture("BrickNormal");
	scifiPanelPlane->heightMap = resource_manager::GetTexture("BrickHeight");

	material_pbr* firstElement = new material_pbr();
	firstElement->albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	firstElement->metallic = 0.65f;
	firstElement->roughness = .5f;

	material_pbr* secondElement = new material_pbr();
	secondElement->albedo = glm::vec3(0.8f, 0.0f, 0.0f);
	secondElement->metallic = .8f;
	secondElement->roughness = 0.25f;

	transparent_pbr* thirdElement = new transparent_pbr();
	thirdElement->albedo = glm::vec3(0.57f, 0.73f, 1.f);
	thirdElement->metallic = 1.f;
	thirdElement->roughness = 0.f;
	thirdElement->alpha = 0.2f;

	material_pbr* fourthElement = new material_pbr();
	fourthElement->albedo = glm::vec3(.24f);
	fourthElement->metallic = 0.1f;
	fourthElement->roughness = 0.75f;

	material_pbr* fifthElement = new material_pbr();
	fifthElement->albedo = glm::vec3(0.8f, 0.58f, .0f);
	fifthElement->metallic = .0f;
	fifthElement->roughness = .5f;

	transparent_pbr* sixthElement = new transparent_pbr();
	sixthElement->albedo = glm::vec3(0.0f, 0.0f, .0f);
	sixthElement->roughness = .064f;
	sixthElement->metallic = .92f;
	sixthElement->alpha = .85f;

	material_pbr* seventhElement = new material_pbr();
	seventhElement->albedo = glm::vec3(0.8f, 0.4f, .0f);
	seventhElement->metallic = .7f;
	seventhElement->roughness = .5f;

	material_pbr* eighthElement = new material_pbr();
	eighthElement->albedo = glm::vec3(0.8f, 0.0f, .0f);
	eighthElement->metallic = .1f;
	eighthElement->roughness = .4f;

	transparent_pbr* ninthElement = new transparent_pbr();
	ninthElement->albedo = glm::vec3(0.8f, 0.0f, .0f);
	ninthElement->metallic = .6f;
	ninthElement->roughness = .22f;
	ninthElement->alpha = .5f;

	material_pbr* tenthElement = new material_pbr();
	tenthElement->albedo = glm::vec3(0.8f, 0.77f, .47f);
	tenthElement->metallic = .65f;
	tenthElement->roughness = .1f;

	material_pbr* eleventhElement = new material_pbr();
	eleventhElement->albedo = glm::vec3(0.4f, 0.0f, .0f);
	eleventhElement->metallic = .0f;
	eleventhElement->roughness = .82f;

	material_lit_no_textures* twelfthElement = new material_lit_no_textures();
	twelfthElement->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	material_lit_no_textures* twelfthElement2 = new material_lit_no_textures();
	twelfthElement2->diffuse = glm::vec3(0.8f, 0.0f, 0.0f);
	twelfthElement2->specular = glm::vec3(0.0f, 0.0f, 0.8f);

	std::vector<material*> materials = {twelfthElement, twelfthElement2, twelfthElement, twelfthElement2, twelfthElement, twelfthElement2,
										twelfthElement, twelfthElement2, twelfthElement, twelfthElement,  twelfthElement, twelfthElement};

	// Entity sphere0 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(sphere0, transform {glm::vec4(-5, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(sphere0, mesh_renderer {resource_manager::GetModel("SPHERE"), {sand}});
	//
	// Entity sphere1 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(sphere1, transform {glm::vec4(-3, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(sphere1, mesh_renderer {resource_manager::GetModel("SPHERE"), {rusted}});
	//
	// Entity sphere2 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(sphere2, transform {glm::vec4(-1, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(sphere2, mesh_renderer {resource_manager::GetModel("SPHERE"), {wood}});
	//
	// Entity sphere3 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(sphere3, transform {glm::vec4(1, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(sphere3, mesh_renderer {resource_manager::GetModel("SPHERE"), {fabric}});
	//
	// Entity sphere4 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(sphere4, transform {glm::vec4(3, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(sphere4, mesh_renderer {resource_manager::GetModel("SPHERE"), {spaceBlanket}});
	//
	// Entity sphere5 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(sphere5, transform {glm::vec4(5, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(sphere5, mesh_renderer {resource_manager::GetModel("SPHERE"), {acousticFoam}});
	//
	// Entity plane = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(plane, transform {glm::vec4(0, -8, 0, 1), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)});
	// gCoordinator.AddComponent(plane, mesh_renderer {resource_manager::GetModel("PLANE"), {twelfthElement}});

	Entity plane2 = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(plane2, transform {glm::vec4(0, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	gCoordinator.AddComponent(plane2, mesh_renderer {resource_manager::GetModel("Nissan2"), materials});

	// Entity plane3 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(plane2, transform {glm::vec4(10, 0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(plane2, mesh_renderer {resource_manager::GetModel("Nissan"), materials});

	// Entity e = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e, transform {glm::vec3(0, 1, 2), glm::vec3(0, 30, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e, mesh_renderer {resource_manager::GetModel("box"), mat, resource_manager::GetShader("default_lit")});
	// Entity e2 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e2, transform {glm::vec3(0, -2, 0), glm::vec3(0, 0, 0), glm::vec3(100, 100, 100)});
	// gCoordinator.AddComponent(e2, mesh_renderer {resource_manager::GetModel("PLANE"), spaceBlanket, resource_manager::GetShader("default_lit")});
	// Entity e3 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(e3, transform {glm::vec3(-2, 4, 1), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	// gCoordinator.AddComponent(e3, mesh_renderer {resource_manager::GetModel("SPHERE"), sphereMat, resource_manager::GetShader("default_lit")});
	//    Entity e4 = gCoordinator.CreateEntity();
	//    gCoordinator.AddComponent(e4, transform {glm::vec3(4, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	//    gCoordinator.AddComponent(e4, mesh_renderer {resource_manager::GetModel("backpack"), matBackpack, resource_manager::GetShader("default_lit")});
	//     Entity e4 = gCoordinator.CreateEntity();
	//     gCoordinator.AddComponent(e4, transform {glm::vec3(-10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	//     gCoordinator.AddComponent(e4, mesh_renderer {resource_manager::GetModel("backpack"), mat, resource_manager::GetShader("default_lit")});
	//     Entity e5 = gCoordinator.CreateEntity();
	//     gCoordinator.AddComponent(e5, transform {glm::vec3(5, 0, 5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	//     gCoordinator.AddComponent(e5, mesh_renderer {resource_manager::GetModel("backpack"), mat, resource_manager::GetShader("default_lit")});
	//     Entity e6 = gCoordinator.CreateEntity();
	//     gCoordinator.AddComponent(e6, transform {glm::vec3(5, 0, -5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	//     gCoordinator.AddComponent(e6, mesh_renderer {resource_manager::GetModel("backpack"), mat, resource_manager::GetShader("default_lit")});
	//     Entity e7 = gCoordinator.CreateEntity();
	//     gCoordinator.AddComponent(e7, transform {glm::vec3(-5, 0, -5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	//     gCoordinator.AddComponent(e7, mesh_renderer {resource_manager::GetModel("backpack"), mat, resource_manager::GetShader("default_lit")});
	//     Entity e8 = gCoordinator.CreateEntity();
	//     gCoordinator.AddComponent(e8, transform {glm::vec3(-5, 0, 5), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)});
	//     gCoordinator.AddComponent(e8, mesh_renderer {resource_manager::GetModel("backpack"), mat, resource_manager::GetShader("default_lit")});

	Entity fpsCamera = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(fpsCamera, fps_camera {cameraSystem.GetCurrentCamera()});
	gCoordinator.AddComponent(fpsCamera, camera_movement {cameraSystem.GetCurrentCamera()});

	// material_unlit* redMat = new material_unlit();
	// redMat->map = resource_manager::GetTexture("DEFAULT");
	// redMat->ambient = glm::vec3(1.0f, 0.0f, 0.0f);
	//
	// material_unlit* blueMat = new material_unlit();
	// blueMat->map = resource_manager::GetTexture("DEFAULT");
	// blueMat->ambient = glm::vec3(0.0f, 0.0f, 1.0f);

	Entity directionalLight = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(directionalLight, directional_light(glm::vec3(0, -1, -.8f), 25.f));

	gCoordinator.AddComponent(directionalLight, light(1.f, 100.f, 1.f, light::DIRECTIONAL));

	Entity sky = gCoordinator.CreateEntity();
	gCoordinator.AddComponent(sky, skybox {resource_manager::GetCubemap("skybox")});

	// Entity pointLight = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(pointLight, transform {glm::vec4(-3, 2, 2, 1), glm::vec3(0, 0, 0), glm::vec3(.2f)});
	// gCoordinator.AddComponent(pointLight, point_light {30.f, true, glm::vec3(1.f, 0.f, 0.f)});
	// gCoordinator.AddComponent(pointLight, mesh_renderer {resource_manager::GetModel("SPHERE"), {redMat}});
	//
	// Entity pointLight2 = gCoordinator.CreateEntity();
	// gCoordinator.AddComponent(pointLight2, transform {glm::vec4(3, 2, 2, 1), glm::vec3(0, 0, 0), glm::vec3(.2f)});
	// gCoordinator.AddComponent(pointLight2, point_light {40.f, true, glm::vec3(0.f, 0.f, 1.f)});
	// gCoordinator.AddComponent(pointLight2, mesh_renderer {resource_manager::GetModel("SPHERE"), {blueMat}});

	pointLightManager->Init();
	directionalLightManager->Init();
	skyboxSystem->Init();
	lightManager->Init();
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

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<skybox>());
	gCoordinator.SetSystemSignature<skybox_system>(sig);
	skyboxSystem = gCoordinator.RegisterSystem<skybox_system>();

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<transform>());
	gCoordinator.SetSystemSignature<transform_system>(sig);
	transformSystem = gCoordinator.RegisterSystem<transform_system>();

	sig = Signature();
	sig.set(gCoordinator.GetComponentType<light>() &
			(gCoordinator.GetComponentType<directional_light>() | (gCoordinator.GetComponentType<point_light>() & gCoordinator.GetComponentType<transform>())));
	gCoordinator.SetSystemSignature<light_manager>(sig);
	lightManager = gCoordinator.RegisterSystem<light_manager>();
}

void engine::Update()
{
	MousePositionUpdate();
	fpsCameraSystem->Update(deltaTime);
	cameraMovementSystem->Update(deltaTime);
	transformSystem->Update();

	meshRendererSystem->Update();
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

	// directionalLightManager->SetShaderVariables();
	// pointLightManager->SetShaderVariables();
	lightManager->SetShaderVariables();
	SetShaderVariables();
	meshRendererSystem->Render(false);
	// meshRendererSystem->Render(true);
	//   m->t.rotation = glm::vec3(0, glm::radians((float)counter), 0);
	//   m->Render(resource_manager::GetShader("default"));
	glDepthFunc(GL_LEQUAL);
	skyboxSystem->Render();
	glDepthFunc(GL_LESS);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glBlitFramebuffer(0, 0, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputHeight, 0, 0, windowInfo.windowWidth, windowInfo.windowHeight, GL_COLOR_BUFFER_BIT,
					  GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void engine::SetShaderVariables()
{
	cameraSystem.GetCurrentCamera()->ComputeViewMatrix();
	glm::mat4 view = cameraSystem.GetCurrentCamera()->view;
	glm::mat4 projection = cameraSystem.GetCurrentCamera()->projection;

	resource_manager::GetShader("default_pbr")->SetMatrix4("view", view, true);
	resource_manager::GetShader("default_pbr")->SetMatrix4("projection", projection);
	resource_manager::GetShader("default_pbr")->SetVector3f("viewPos", cameraSystem.GetCurrentCamera()->position);

	resource_manager::GetShader("default_lit")->SetMatrix4("view", view, true);
	resource_manager::GetShader("default_lit")->SetMatrix4("projection", projection);
	resource_manager::GetShader("default_lit")->SetVector3f("viewPos", cameraSystem.GetCurrentCamera()->position);

	resource_manager::GetShader("default_lit_no_textures")->SetMatrix4("view", view, true);
	resource_manager::GetShader("default_lit_no_textures")->SetMatrix4("projection", projection);
	resource_manager::GetShader("default_lit_no_textures")->SetVector3f("viewPos", cameraSystem.GetCurrentCamera()->position);

	resource_manager::GetShader("transparent_pbr")->SetMatrix4("view", view, true);
	resource_manager::GetShader("transparent_pbr")->SetMatrix4("projection", projection);
	resource_manager::GetShader("transparent_pbr")->SetVector3f("viewPos", cameraSystem.GetCurrentCamera()->position);

	resource_manager::GetShader("default_unlit")->SetMatrix4("view", view, true);
	resource_manager::GetShader("default_unlit")->SetMatrix4("projection", projection);

	resource_manager::GetShader("skybox")->SetMatrix4("view", glm::mat4(glm::mat3(view)), true);
	resource_manager::GetShader("skybox")->SetMatrix4("projection", projection);
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

		if (opt.centerWindow)
		{
			glm::vec2 centerOfMonitor = glm::vec2(windowInfo.monitorWidth / 2.f, windowInfo.monitorHeight / 2.f);
			glm::vec2 halfWindowDimensions = glm::vec2(windowInfo.windowWidth / 2.f, windowInfo.windowHeight / 2.f);
			glm::vec2 pos = centerOfMonitor - halfWindowDimensions;
			glfwSetWindowPos(window, pos.x, pos.y);
		}
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
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
	glfwSwapInterval(opt.vSync);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_CULL_FACE);
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
	glGenFramebuffers(1, &GBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, GBUFFER);
	glGenTextures(TEXTURE_COUNT, GTEX);

	for (int i = 0; i < TEXTURE_COUNT; i++)
	{
		glBindTexture(GL_TEXTURE_2D, GTEX[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputWidth, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, GTEX[i], 0);
	}

	glGenRenderbuffers(1, &DEPTH);
	glBindRenderbuffer(GL_RENDERBUFFER, DEPTH);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, cameraSystem.GetCurrentCamera()->outputWidth, cameraSystem.GetCurrentCamera()->outputWidth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DEPTH);

	unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);
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
