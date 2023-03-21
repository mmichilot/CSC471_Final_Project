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
	string audioDir    = resourceDir + "/audio";


	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application application = Application();

	// Setup window and callbacks
	WindowManager windowManager = WindowManager();
	windowManager.init(WIDTH, HEIGHT);
	windowManager.setEventCallbacks(&application);
	application.windowManager = &windowManager;

	// Initilize application
	application.init();
	application.initShaders(shaderDir);
	application.initGeometry(objectDir);
	application.initTextures(textureDir);
	application.initAudio(audioDir);
	application.initLights();
	application.initShadows();
	application.initCameras();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager.getHandle()))
	{
		application.render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager.getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager.shutdown();
	return 0;
}
