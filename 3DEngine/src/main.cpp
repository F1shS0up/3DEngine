#include "GLFW/glfw3.h"
#include "engine.h"
#include "glad/glad.h"
#include "irrKlang.h"

#include <chrono>
#include <iostream>

double frameStart, frameEnd;

int main(int argc, char* argv[])
{
	float deltaTime = 1.f;
	engine* e = new engine(deltaTime);

	e->Init();
	float time = 0;

	while (!glfwWindowShouldClose(e->window) && e->running)
	{
		frameStart = glfwGetTime();
		e->Update();
		e->Render();

		glfwSwapBuffers(e->window);
		glfwPollEvents();

		frameEnd = glfwGetTime();
		deltaTime = (frameEnd - frameStart);
		time += deltaTime;

		if (time >= 1.f)
		{
			time = 0;
			std::cout << 1.f / deltaTime << std::endl;
		}
	}
	e->Terminate();
	glfwTerminate();
	return 0;
}