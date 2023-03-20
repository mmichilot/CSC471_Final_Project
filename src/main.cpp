#include "Application.h"
#include "common.h"

#define WIDTH  1280
#define HEIGHT 720

using namespace std;
	

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	string resourceDir = "../../resources";
	string shaderDir   = resourceDir + "/shaders";
	string textureDir  = resourceDir + "/textures";
	string objectDir   = resourceDir + "/objects";


	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Setup window and callbacks
	WindowManager *windowManager = new WindowManager();
	windowManager->init(WIDTH, HEIGHT);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// Initilize application
	application->init();
	application->initShaders(shaderDir);
	application->initGeometry(objectDir);
	application->initTextures(textureDir);
	application->initLights();
	application->initShadows();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
