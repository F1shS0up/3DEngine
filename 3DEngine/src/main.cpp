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

	while (!glfwWindowShouldClose(e->window) && e->running)
	{
		frameStart = glfwGetTime();
		e->Update();
		e->Render();

		glfwSwapBuffers(e->window);
		glfwPollEvents();

		frameEnd = glfwGetTime();
		deltaTime = (frameEnd - frameStart);
	}
	e->Terminate();
	glfwTerminate();
	return 0;
}